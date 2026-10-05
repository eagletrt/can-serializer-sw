#ifndef GS_USB_PROTOCOL_H
#define GS_USB_PROTOCOL_H

#include <stdint.h>

enum GsUsbRequest {
    GS_USB_REQUEST_HOST_FORMAT = 0,
    GS_USB_REQUEST_BIT_TIMING = 1,
    GS_USB_REQUEST_MODE = 2,
    GS_USB_REQUEST_BIT_TIMING_CONSTANTS = 4,
    GS_USB_REQUEST_DEVICE_CONFIG = 5,
    GS_USB_REQUEST_DATA_BIT_TIMING = 10,
    GS_USB_REQUEST_BIT_TIMING_CONSTANTS_EXTENDED = 11,
};

enum GsUsbMode {
    GS_USB_MODE_RESET = 0,
    GS_USB_MODE_START = 1,
};

enum GsUsbFeature {
    GS_USB_FEATURE_LISTEN_ONLY = (1u << 0),
    GS_USB_FEATURE_LOOP_BACK = (1u << 1),
    GS_USB_FEATURE_TRIPLE_SAMPLE = (1u << 2),
    GS_USB_FEATURE_ONE_SHOT = (1u << 3),
    GS_USB_FEATURE_HARDWARE_TIMESTAMP = (1u << 4),
    GS_USB_FEATURE_FD = (1u << 8),
    GS_USB_FEATURE_BIT_TIMING_CONSTANTS_EXTENDED = (1u << 10),
};

struct GsUsbHostConfiguration {
    uint32_t byte_order;
};

struct GsUsbDeviceConfiguration {
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t channel_count;
    uint32_t software_version;
    uint32_t hardware_version;
};

struct GsUsbBitTiming {
    uint32_t propagation_segment;
    uint32_t phase_segment1;
    uint32_t phase_segment2;
    uint32_t sync_jump_width;
    uint32_t bitrate_prescaler;
};

struct GsUsbModeRequest {
    uint32_t mode;
    uint32_t flags;
};

struct GsUsbBitTimingConstants {
    uint32_t feature;
    uint32_t can_clock_frequency;
    uint32_t timing_segment1_min, timing_segment1_max;
    uint32_t timing_segment2_min, timing_segment2_max;
    uint32_t sync_jump_width_max;
    uint32_t bitrate_prescaler_min, bitrate_prescaler_max, bitrate_prescaler_inc;
};

struct GsUsbBitTimingConstantsExtended {
    struct GsUsbBitTimingConstants nominal;
    uint32_t data_timing_segment1_min, data_timing_segment1_max;
    uint32_t data_timing_segment2_min, data_timing_segment2_max;
    uint32_t data_sync_jump_width_max;
    uint32_t data_bitrate_prescaler_min, data_bitrate_prescaler_max, data_bitrate_prescaler_inc;
};

#endif // GS_USB_PROTOCOL_H
