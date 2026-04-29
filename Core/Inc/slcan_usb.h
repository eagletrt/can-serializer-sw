#ifndef USBD_CDC_H
#define USBD_CDC_H

#include <stdint.h>

void cdc_transmit(uint8_t itf, uint8_t *buf, uint16_t len);
void cdc_process(void);

#endif // USBD_CDC_H
