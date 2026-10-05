#include "gs_usb_tinyusb.h"
#include "tusb.h"

static struct GsUsbTinyUsbHandler tinyusb_handler;

void gs_usb_tinyusb_init(struct GsUsbCore *core) {
    tinyusb_handler.core = core;
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    if (tinyusb_handler.core == NULL || request->bmRequestType_bit.type != TUSB_REQ_TYPE_VENDOR)
        return false;

    const bool direction_in = (request->bmRequestType_bit.direction == TUSB_DIR_IN);

    if (stage == CONTROL_STAGE_SETUP) {
        void *buffer;
        uint16_t buffer_length;
        if (gs_usb_control_setup(tinyusb_handler.core,
                                 request->bRequest,
                                 request->wValue,
                                 request->wLength,
                                 direction_in,
                                 &buffer,
                                 &buffer_length) != GS_USB_CONTROL_STATUS_OK)
            return false;
        return tud_control_xfer(rhport, request, buffer, buffer_length);
    }

    if (stage == CONTROL_STAGE_ACK && !direction_in)
        gs_usb_control_complete(tinyusb_handler.core, request->bRequest, request->wValue);

    return true;
}
