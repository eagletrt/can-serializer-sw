#ifndef GS_USB_TINYUSB_H
#define GS_USB_TINYUSB_H

#include "gs_usb_core.h"

struct GsUsbTinyUsbHandler {
    struct GsUsbCore *core;
};

/* Registers the core that the TinyUSB control callback talks to.
 * Call once at startup, before tusb_init(). The core must outlive the program. */
void gs_usb_tinyusb_init(struct GsUsbCore *core);

#endif // GS_USB_TINYUSB_H
