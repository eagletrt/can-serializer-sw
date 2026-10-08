#include <string.h>

#include "gs_usb_core.h"

//--------------------------------------------------------------------
// Wire format
//--------------------------------------------------------------------
/* Device-wide: the FD size is used as soon as any started channel uses FD.
 * Verify against hf_size_rx / hf_size_tx in the Linux gs_usb.c. */
static uint32_t wire_frame_size(const struct GsCore *core) {
    bool fd = false;
    for (uint8_t i = 0; i < core->capabilities.channel_count; i++) {
        if (core->channels[i].started && (core->channels[i].configuration.mode_flags & GS_USB_FEATURE_FD))
            fd = true;
    }
    return sizeof(struct GsUsbHostFrameHeader) + (fd ? 64 : 8);
}

static struct GsCanFrame frame_from_wire(const struct GsUsbHostFrame *wire) {
    const uint32_t can_id = wire->header.can_id;
    const bool extended = (can_id & GS_USB_CAN_ID_EXTENDED) != 0;

    struct GsCanFrame frame = { 0 };
    frame.identifier = can_id & (extended ? GS_USB_CAN_ID_MASK_EXTENDED : GS_USB_CAN_ID_MASK_STANDARD);
    if (extended)
        frame.flags |= GS_CAN_FRAME_FLAG_EXTENDED;
    if (can_id & GS_USB_CAN_ID_REMOTE)
        frame.flags |= GS_CAN_FRAME_FLAG_REMOTE;
    if (wire->header.flags & GS_USB_FRAME_FLAG_FD)
        frame.flags |= GS_CAN_FRAME_FLAG_FD;
    if (wire->header.flags & GS_USB_FRAME_FLAG_BIT_RATE_SWITCH)
        frame.flags |= GS_CAN_FRAME_FLAG_BIT_RATE_SWITCH;
    if (wire->header.flags & GS_USB_FRAME_FLAG_ERROR_STATE)
        frame.flags |= GS_CAN_FRAME_FLAG_ERROR_STATE;

    frame.dlc = wire->header.can_dlc & 0x0F;
    if (!(frame.flags & GS_CAN_FRAME_FLAG_FD) && frame.dlc > 8)
        frame.dlc = 8;
    memcpy(frame.data, wire->data, gs_can_dlc_to_length(frame.dlc));
    return frame;
}

static void wire_from_frame(struct GsUsbHostFrame *wire, const struct GsCanFrame *frame, uint8_t channel) {
    memset(wire, 0, sizeof *wire);
    wire->header.echo_id = GS_USB_ECHO_ID_RECEIVED;
    wire->header.can_id = frame->identifier;
    if (frame->flags & GS_CAN_FRAME_FLAG_EXTENDED)
        wire->header.can_id |= GS_USB_CAN_ID_EXTENDED;
    if (frame->flags & GS_CAN_FRAME_FLAG_REMOTE)
        wire->header.can_id |= GS_USB_CAN_ID_REMOTE;
    wire->header.can_dlc = frame->dlc;
    wire->header.channel = channel;
    if (frame->flags & GS_CAN_FRAME_FLAG_FD)
        wire->header.flags |= GS_USB_FRAME_FLAG_FD;
    if (frame->flags & GS_CAN_FRAME_FLAG_BIT_RATE_SWITCH)
        wire->header.flags |= GS_USB_FRAME_FLAG_BIT_RATE_SWITCH;
    if (frame->flags & GS_CAN_FRAME_FLAG_ERROR_STATE)
        wire->header.flags |= GS_USB_FRAME_FLAG_ERROR_STATE;
    memcpy(wire->data, frame->data, gs_can_dlc_to_length(frame->dlc));
}

//--------------------------------------------------------------------
// USB OUT -> CAN
//--------------------------------------------------------------------
/* True when the pending frame is finished (sent or dropped), false to retry on the next poll */
static bool submit_pending(struct GsCore *core) {
    const struct GsUsbHostFrameHeader *header = &core->pending.header;

    if (header->channel >= core->capabilities.channel_count)
        return true;
    struct GsChannel *channel = &core->channels[header->channel];

    const bool channel_fd = (channel->configuration.mode_flags & GS_USB_FEATURE_FD) != 0;
    if (!channel->started || header->echo_id >= GS_ECHO_SLOT_COUNT ||
        ((header->flags & GS_USB_FRAME_FLAG_FD) && !channel_fd))
        return true; // dropped, there is no echo for it

    const struct GsCanFrame frame = frame_from_wire(&core->pending);
    if (!core->can_operations->transmit(core->can_context, header->channel, &frame, (uint8_t)header->echo_id))
        return false;

    // The echo is the frame the host sent. Only gs_core_poll touches it, so there is no race.
    channel->echo[header->echo_id] = core->pending;
    return true;
}

static void poll_usb_out(struct GsCore *core) {
    const struct GsUsbOperations *usb = core->usb_operations;
    const uint32_t size = wire_frame_size(core);

    for (;;) {
        if (!core->has_pending) {
            if (usb->out_available(core->usb_context) < size)
                return;
            memset(&core->pending, 0, sizeof core->pending);
            usb->out_read(core->usb_context, &core->pending, size);
            core->has_pending = true;
        }
        if (!submit_pending(core))
            return; // CAN queue full: hold the frame and retry on the next poll
        core->has_pending = false;
    }
}

//--------------------------------------------------------------------
// CAN -> USB IN
//--------------------------------------------------------------------
/* False if the IN endpoint ran out of space */
static bool drain_channel(struct GsCore *core, uint8_t index, uint32_t size) {
    const struct GsUsbOperations *usb = core->usb_operations;
    const struct GsCanOperations *can = core->can_operations;
    const struct GsChannel *channel = &core->channels[index];

    if (!channel->started)
        return true;

    for (;;) {
        if (usb->in_space(core->usb_context) < size)
            return false;

        struct GsUsbHostFrame wire;
        struct GsCanFrame frame;
        uint8_t cookie;

        if (can->transmitted(core->can_context, index, &cookie)) {
            if (cookie >= GS_ECHO_SLOT_COUNT)
                continue;
            wire = channel->echo[cookie]; // the echo is the frame the host sent, unchanged
        } else if (can->receive(core->can_context, index, &frame)) {
            wire_from_frame(&wire, &frame, index);
        } else {
            return true;
        }
        usb->in_write(core->usb_context, &wire, size);
    }
}

void gs_core_poll(struct GsCore *core) {
    const uint8_t count = core->capabilities.channel_count;
    if (count == 0)
        return;
    const uint32_t size = wire_frame_size(core);

    poll_usb_out(core);

    for (uint8_t n = 0; n < count; n++) {
        if (!drain_channel(core, (uint8_t)((core->next_channel + n) % count), size))
            break;
    }
    core->next_channel = (uint8_t)((core->next_channel + 1) % count);
    core->usb_operations->in_flush(core->usb_context);
}
