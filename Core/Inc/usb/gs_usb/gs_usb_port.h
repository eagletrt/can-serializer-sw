#ifndef GS_PORT_H
#define GS_PORT_H

#include <stdint.h>

#include "gs_usb_protocol.h"

#define GS_CAN_DATA_LENGTH_MAX (64)

enum GsCanFrameFlag {
    GS_CAN_FRAME_FLAG_EXTENDED = (1u << 0),
    GS_CAN_FRAME_FLAG_REMOTE = (1u << 1),
    GS_CAN_FRAME_FLAG_FD = (1u << 2),
    GS_CAN_FRAME_FLAG_BIT_RATE_SWITCH = (1u << 3),
    GS_CAN_FRAME_FLAG_ERROR_STATE = (1u << 4),
};

/* Frame as the core sees it: independent of the wire format and of any HAL */
struct GsCanFrame {
    uint32_t identifier; /*<! 11 or 29 bit, no flag bits mixed in */
    uint8_t dlc;         /*<! DLC code 0..15 */
    uint8_t flags;       /*<! GsCanFrameFlag bits */
    uint8_t data[GS_CAN_DATA_LENGTH_MAX];
};

static inline uint8_t gs_can_dlc_to_length(uint8_t dlc) {
    static const uint8_t lengths[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64 };
    return lengths[dlc & 0x0F];
}

/* Timing as the CAN peripheral wants it, not as the wire carries it */
struct GsCanTiming {
    uint32_t bitrate_prescaler;
    uint32_t timing_segment1; /*<! propagation segment + phase segment 1 */
    uint32_t timing_segment2;
    uint32_t sync_jump_width;
};

struct GsCanChannelConfiguration {
    struct GsCanTiming nominal;
    struct GsCanTiming data;
    uint32_t mode_flags; /*<! GsUsbFeature bits, already checked against the capabilities */
};

/* What the platform really supports. The core builds BT_CONST from this. */
struct GsCanCapabilities {
    uint32_t clock_frequency; /*<! Hz, after any dividers: what the CAN core really runs at */
    uint32_t features;        /*<! GsUsbFeature bits the port implements */
    struct GsCanTiming nominal_min, nominal_max;
    uint32_t nominal_bitrate_prescaler_increment;
    struct GsCanTiming data_min, data_max;
    uint32_t data_bitrate_prescaler_increment;
    uint8_t channel_count; /*<! real count, the core subtracts one for the wire */
    uint32_t software_version;
    uint32_t hardware_version;
};

/* All operations are called from the main loop only, never from an ISR */
struct GsCanOperations {
    /* Must work again after stop() on the same channel. False if the peripheral rejects it. */
    bool (*start)(void *context, uint8_t channel, const struct GsCanChannelConfiguration *configuration);
    /* Only affects its own channel, safe to call on a channel that was never started */
    void (*stop)(void *context, uint8_t channel);
    /* Queue a frame; the cookie must come back through transmitted(). False only if the queue is full. */
    bool (*transmit)(void *context, uint8_t channel, const struct GsCanFrame *frame, uint8_t cookie);
    /* Pop one received frame. False if there is none. */
    bool (*receive)(void *context, uint8_t channel, struct GsCanFrame *frame);
    /* Pop the cookie of one frame that has been transmitted. False if there is none. */
    bool (*transmitted)(void *context, uint8_t channel, uint8_t *cookie);
};

/* Non-blocking view of the bulk endpoints as byte streams */
struct GsUsbOperations {
    uint32_t (*out_available)(void *context);
    uint32_t (*out_read)(void *context, void *destination, uint32_t length);
    uint32_t (*in_space)(void *context);
    uint32_t (*in_write)(void *context, const void *source, uint32_t length);
    void (*in_flush)(void *context);
};

#endif // GS_PORT_H
