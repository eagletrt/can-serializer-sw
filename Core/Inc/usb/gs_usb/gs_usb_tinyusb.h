#ifndef GS_USB_TINYUSB_H
#define GS_USB_TINYUSB_H

#include "gs_usb_core.h"

/* Registers the core that the TinyUSB callbacks talk to.
 * Call after gs_core_initialize() and before tusb_init(). The core must outlive the program. */
void gs_tinyusb_initialize(struct GsCore *core);

const struct GsUsbOperations *gs_tinyusb_get_operations(void);

/* Replaces tud_task() in the main loop: runs the USB stack, then the gs_usb data plane */
void gs_tinyusb_task(void);

#endif // GS_USB_TINYUSB_H
