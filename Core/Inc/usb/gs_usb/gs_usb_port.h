#ifndef GS_USB_PORT_H
#define GS_USB_PORT_H

#include <stdbool.h>
#include <stdint.h>

#include "gs_usb_protocol.h"

/* Timing as the CAN peripheral wants it, not as the wire carries it. */
struct GsUsbTiming {
    uint32_t bitrate_prescaler;
    uint32_t timing_segment1; /*<! propagation segment + phase segment 1 */
    uint32_t timing_segment2;
    uint32_t sync_jump_width;
};

struct GsUsbChannelConfiguration {
    struct GsUsbTiming nominal;
    struct GsUsbTiming data;
    uint32_t mode_flags; /*<! GsUsbFeature bits, already checked against the capabilities */
};

/* What the platform really supports. The core builds BT_CONST from this. */
struct GsUsbCapabilities {
    uint32_t clock_frequency; /*<! Hz, after any dividers: what the CAN core really runs at */
    uint32_t features;        /*<! GsUsbFeature bits the port implements */
    struct GsUsbTiming nominal_min, nominal_max;
    uint32_t nominal_bitrate_prescaler_increment;
    struct GsUsbTiming data_min, data_max;
    uint32_t data_bitrate_prescaler_increment;
    uint8_t channel_count; /*<! real count, the core subtracts one for the wire */
    uint32_t software_version;
    uint32_t hardware_version;
};

typedef bool (*gs_usb_start_callback)(void *context, uint8_t channel, const struct GsUsbChannelConfiguration *configuration);
typedef void (*gs_usb_stop_callback)(void *context, uint8_t channel);

struct GsUsbOperations {
    gs_usb_start_callback start;
    gs_usb_stop_callback stop;
};

#endif // GS_USB_PORT_H
