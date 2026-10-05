#include <string.h>

#include "tusb.h"

#define USB_VID (0x1D50)
#define USB_PID (0x606F)

#define ENDPOINT_GS_IN (0x81)
#define ENDPOINT_GS_OUT (0x02)

#define CONFIGURATION_TOTAL_LENGTH (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)

enum Interface : uint8_t {
    INTERFACE_GS,
    INTERFACE_COUNT
};

enum StringId : uint8_t {
    STRING_ID_LANGUAGE_ID,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    STRING_ID_SERIAL_NUMBER,
    STRING_ID_INTERFACE
};

static uint16_t descriptor_string[32 + 1];

//--------------------------------------------------------------------
// Device descriptor
//--------------------------------------------------------------------
static tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = STRING_ID_MANUFACTURER,
    .iProduct = STRING_ID_PRODUCT,
    .iSerialNumber = STRING_ID_SERIAL_NUMBER,
    .bNumConfigurations = 0x01
};

//--------------------------------------------------------------------
// Configuration descriptor
//--------------------------------------------------------------------
static uint8_t const configuration_descriptor[] = {
    // config number, interface count, string index, total length, attributes, power (mA)
    TUD_CONFIG_DESCRIPTOR(1, INTERFACE_COUNT, 0, CONFIGURATION_TOTAL_LENGTH, 0x00, 100),

    // interface number, string index, EP OUT, EP IN, EP size
    // Produces class/subclass/protocol 0xFF/0xFF/0xFF
    TUD_VENDOR_DESCRIPTOR(INTERFACE_GS, STRING_ID_INTERFACE, ENDPOINT_GS_OUT, ENDPOINT_GS_IN, 64),
};

//--------------------------------------------------------------------
// String descriptors
//--------------------------------------------------------------------
static const char *const string_descriptor_buffer[] = {
    (const char[]){ 0x09, 0x04 }, // 0: Supported language is English (0x0409)
    "E-AGLE TRT",                 // 1: Manufacturer
    "CAN serializer",             // 2: Product
    "DIOCAN",                     // 3: Serial Number
    "gs_usb Interface"            // 4: Interface Name
};

uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *)&desc_device;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;

    return configuration_descriptor;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t language_id) {
    (void)language_id;
    size_t character_count;

    if (index == STRING_ID_LANGUAGE_ID) {
        memcpy(&descriptor_string[1], string_descriptor_buffer[0], 2);
        character_count = 1;
    } else {
        if (index >= sizeof(string_descriptor_buffer) / sizeof(string_descriptor_buffer[0]))
            return NULL;
        const char *string = string_descriptor_buffer[index];
        character_count = strlen(string);
        if (character_count > 32)
            character_count = 32;
        for (size_t i = 0; i < character_count; i++)
            descriptor_string[1 + i] = string[i];
    }

    descriptor_string[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * character_count + 2));

    return descriptor_string;
}
