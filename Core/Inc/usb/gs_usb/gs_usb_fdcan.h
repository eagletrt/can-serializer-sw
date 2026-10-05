#ifndef GS_USB_FDCAN_H
#define GS_USB_FDCAN_H

#include "gs_usb_port.h"
#include "fdcan.h"

enum FdcanPortChannel {
    FDCAN_PORT_CHANNEL_PRIMARY,
    FDCAN_PORT_CHANNEL_SECONDARY,
    FDCAN_PORT_CHANNEL_INVERTER,
    FDCAN_PORT_CHANNEL_COUNT,
};

struct GsUsbFdcanPortChannel {
    FDCAN_HandleTypeDef *hfdcan;
    uint32_t message_ram_offset; /*<! in words, ranges of different channels must not overlap */
};

struct GsUsbFdcanPort {
    struct GsUsbFdcanPortChannel channels[FDCAN_PORT_CHANNEL_COUNT];
};

const struct GsUsbOperations *fdcan_get_operations(void);

/* The clock is read from the RCC at call time, so call this after the clock tree is configured. */
struct GsUsbCapabilities fdcan_get_capabilities(void);

#endif // GS_USB_FDCAN_H
