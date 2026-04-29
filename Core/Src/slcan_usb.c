#include "cdc_device.h"
#include "tusb.h"
#include "tusb_config.h"
#include "slcan.h"
#include "slcan_usb.h"
#include <stdint.h>

uint8_t slcan_string[CFG_TUD_CDC][SLCAN_MAXIMUM_TRANSMISSION_UNIT];
uint8_t slcan_string_idx[CFG_TUD_CDC] = { 0 };

void cdc_transmit(uint8_t itf, uint8_t *buf, uint16_t len) {
    if (tud_cdc_n_connected(itf)) {
        tud_cdc_n_write(itf, buf, len);
        tud_cdc_n_write_flush(itf);
    }
}

void cdc_process(void) {
    for (uint8_t itf = 0; itf < CFG_TUD_CDC; itf++) {
        while (tud_cdc_n_available(itf)) {
            uint8_t local_buf[64];
            uint32_t count = tud_cdc_n_read(itf, local_buf, sizeof(local_buf));
            for (uint32_t i = 0; i < count; i++) {
                uint8_t ch = local_buf[i];
                if (ch == '\r') {
                    slcan_parse_string(itf, slcan_string[itf], slcan_string_idx[itf]);
                    slcan_string_idx[itf] = 0;
                } else {
                    if (slcan_string_idx[itf] < (SLCAN_MAXIMUM_TRANSMISSION_UNIT - 1)) {
                        slcan_string[itf][slcan_string_idx[itf]++] = ch;
                    } else {
                        slcan_string_idx[itf] = 0;
                    }
                }
            }
        }
    }
}
