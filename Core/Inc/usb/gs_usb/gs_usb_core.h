#ifndef GS_USB_CORE_H
#define GS_USB_CORE_H

#include <stdbool.h>
#include <stdint.h>

#include "gs_usb_port.h"
#include "gs_usb_protocol.h"

#ifndef GS_USB_CHANNEL_COUNT_MAX
#define GS_USB_CHANNEL_COUNT_MAX (3)
#endif

#define GS_USB_CONTROL_BUFFER_WORDS (sizeof(struct GsUsbBitTimingConstantsExtended) / sizeof(uint32_t))

enum GsUsbControlStatus {
    GS_USB_CONTROL_STATUS_STALL,
    GS_USB_CONTROL_STATUS_OK,
};

struct GsUsbChannel {
    struct GsUsbChannelConfiguration configuration;
    bool started;
};

struct GsUsbCore {
    struct GsUsbCapabilities capabilities;
    const struct GsUsbOperations *can_operations;
    void *can_context;
    struct GsUsbChannel channels[GS_USB_CHANNEL_COUNT_MAX];
    uint32_t host_byte_order;
    uint32_t control_buffer[GS_USB_CONTROL_BUFFER_WORDS]; /*<! 4-byte aligned, must outlive the transfer */
};

/* Copies the capabilities, clears all channels. channel_count is clamped to GS_CHANNEL_COUNT_MAX. */
void gs_usb_core_init(struct GsUsbCore *core, const struct GsUsbCapabilities *capabilities, const struct GsUsbOperations *can_operations, void *can_context);

/* SETUP stage. On OK, *buffer and *buffer_length describe the data phase:
 * IN requests: already filled with the response.
 * OUT requests: where the received data will be placed. */
enum GsUsbControlStatus gs_usb_control_setup(struct GsUsbCore *core, uint8_t request, uint16_t value, uint16_t length, bool direction_in, void **buffer, uint16_t *buffer_length);

/* ACK stage of an OUT request: the data is in the buffer, apply it. */
void gs_usb_control_complete(struct GsUsbCore *core, uint8_t request, uint16_t value);

#endif // GS_USB_CORE_H
