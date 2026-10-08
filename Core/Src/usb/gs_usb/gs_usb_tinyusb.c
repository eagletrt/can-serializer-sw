#include "gs_usb_tinyusb.h"
#include "tusb.h"

struct GsTinyUsbHandler {
    struct GsCore *core;
};

static struct GsTinyUsbHandler tinyusb_handler;

void gs_tinyusb_initialize(struct GsCore *core) {
    tinyusb_handler.core = core;
}

//--------------------------------------------------------------------
// Control plane
//--------------------------------------------------------------------
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    if (tinyusb_handler.core == NULL || request->bmRequestType_bit.type != TUSB_REQ_TYPE_VENDOR)
        return false;

    const bool direction_in = (request->bmRequestType_bit.direction == TUSB_DIR_IN);

    if (stage == CONTROL_STAGE_SETUP) {
        void *buffer;
        uint16_t buffer_length;
        if (gs_control_setup(tinyusb_handler.core, request->bRequest, request->wValue, request->wLength, direction_in, &buffer, &buffer_length) !=
            GS_CONTROL_STATUS_OK)
            return false;
        return tud_control_xfer(rhport, request, buffer, buffer_length);
    }

    if (stage == CONTROL_STAGE_ACK && !direction_in)
        gs_control_complete(tinyusb_handler.core, request->bRequest, request->wValue);

    return true;
}

//--------------------------------------------------------------------
// Data plane: the bulk endpoints as byte streams
//--------------------------------------------------------------------
static uint32_t usb_out_available(void *context) {
    (void)context;
    return tud_vendor_available();
}

static uint32_t usb_out_read(void *context, void *destination, uint32_t length) {
    (void)context;
    return tud_vendor_read(destination, length);
}

static uint32_t usb_in_space(void *context) {
    (void)context;
    return tud_vendor_write_available();
}

static uint32_t usb_in_write(void *context, const void *source, uint32_t length) {
    (void)context;
    return tud_vendor_write(source, length);
}

static void usb_in_flush(void *context) {
    (void)context;
    tud_vendor_write_flush();
}

static const struct GsUsbOperations usb_operations = {
    .out_available = usb_out_available,
    .out_read = usb_out_read,
    .in_space = usb_in_space,
    .in_write = usb_in_write,
    .in_flush = usb_in_flush,
};

const struct GsUsbOperations *gs_tinyusb_get_operations(void) {
    return &usb_operations;
}

void gs_tinyusb_task(void) {
    tud_task();
    if (tinyusb_handler.core != NULL && tud_mounted())
        gs_core_poll(tinyusb_handler.core);
}
