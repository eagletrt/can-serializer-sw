#ifndef SLCAN_H
#define SLCAN_H

#include <stdint.h>
#include "fdcan.h"

#define SLCAN_FDCAN_MAX_BYTE_LENGTH_DATA (128U)
#define SLCAN_TYPE_BYTE_LENGTH (1U)
#define SLCAN_DLC_BYTE_LENGTH (1U)
#define SLCAN_MAXIMUM_TRANSMISSION_UNIT (SLCAN_TYPE_BYTE_LENGTH + SLCAN_DLC_BYTE_LENGTH + SLCAN_FDCAN_MAX_BYTE_LENGTH_DATA)
#define SLCAN_STANDARD_ID_BYTE_LENGTH (3U)
#define SLCAN_EXTENDED_ID_BYTE_LENGTH (8U)

int8_t hal_dlc_code_byte_length(uint32_t hal_dlc_code);
int32_t slcan_parse_frame(uint8_t *buf, FDCAN_RxHeaderTypeDef *frame_header, uint8_t *frame_data);
int32_t slcan_parse_string(uint8_t *buf, uint8_t len);

#endif
