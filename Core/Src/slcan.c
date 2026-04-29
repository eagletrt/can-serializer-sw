#include "slcan.h"
#include "slcan_usb.h"
#include "fdcan.h"
#include "stm32h7xx_hal_fdcan.h"
#include "eagletrt-api.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/*!
 * \brief Converts a standard 0-F CAN FD data length code to an FDCAN_DLC_BYTES value
 * FIXME: there is no need to do the convertion since the H7 HAL uses non-shifted values
 *
 * \param[in] dlc_code The CAN FD data length code
 * \return A value compatible with the one from the HAL data length code definition
 */
EAGLETRT_STATIC_INLINE uint32_t prv_convert_dlc_code_standard_to_hal(uint8_t dlc_code) {
    return (uint32_t)dlc_code;
}

/*!
 * \brief Converts an FDCAN_DLC_BYTES value to a standard 0-F CAN FD data length code
 * FIXME: there is no need to do the convertion since the H7 HAL uses non-shifted values
 *
 * \param[in] dlc_code An HAL defined data length code (FDCAN_DLC_BYTES)
 * \return A value corresponding to a standard CAN FD data length code
 */
EAGLETRT_STATIC_INLINE uint8_t prv_convert_dlc_code_hal_to_standard(uint32_t dlc_code) {
    return (uint8_t)dlc_code;
}

/*!
 * \brief Converts an FDCAN_DLC_BYTES value to the number of bytes in a message
 *
 * \param[in] hal_dlc_code An HAL defined data length code (FDCAN_DLC_BYTES)
 * \return A value corresponding to the number of bytes associated to a data length code
 */
int8_t hal_dlc_code_byte_length(uint32_t hal_dlc_code) {
    switch (hal_dlc_code) {
        case FDCAN_DLC_BYTES_0:
            return 0;
        case FDCAN_DLC_BYTES_1:
            return 1;
        case FDCAN_DLC_BYTES_2:
            return 2;
        case FDCAN_DLC_BYTES_3:
            return 3;
        case FDCAN_DLC_BYTES_4:
            return 4;
        case FDCAN_DLC_BYTES_5:
            return 5;
        case FDCAN_DLC_BYTES_6:
            return 6;
        case FDCAN_DLC_BYTES_7:
            return 7;
        case FDCAN_DLC_BYTES_8:
            return 8;
        case FDCAN_DLC_BYTES_12:
            return 12;
        case FDCAN_DLC_BYTES_16:
            return 16;
        case FDCAN_DLC_BYTES_20:
            return 20;
        case FDCAN_DLC_BYTES_24:
            return 24;
        case FDCAN_DLC_BYTES_32:
            return 32;
        case FDCAN_DLC_BYTES_48:
            return 48;
        case FDCAN_DLC_BYTES_64:
            return 64;
        default:
            return -1;
    }
}

/*!
 * \brief Parses an incoming CAN frame into an outgoing slcan frame (as an ASCII string)
 *
 * \param[out] buf The buffer containing the slcan frame
 * \param[in] frame_header The CAN frame header containing its description
 * \param[in] frame_data The CAN frame data
 * \return The slcan frame length in bytes
 */
int32_t slcan_parse_frame(uint8_t *buf, FDCAN_RxHeaderTypeDef *frame_header, uint8_t *frame_data) {
    for (uint8_t i = 0; i < SLCAN_MAXIMUM_TRANSMISSION_UNIT; i++)
        buf[i] = '\0';

    uint8_t msg_idx = 0;

    if (frame_header->FDFormat == FDCAN_CLASSIC_CAN) {
        buf[msg_idx] = (frame_header->RxFrameType == FDCAN_REMOTE_FRAME) ? 'r' : 't';
    } else {
        buf[msg_idx] = (frame_header->BitRateSwitch == FDCAN_BRS_ON) ? 'b' : 'd';
    }

    uint8_t id_len = SLCAN_STANDARD_ID_BYTE_LENGTH;
    uint32_t id_tmp = frame_header->Identifier;

    if (frame_header->IdType == FDCAN_EXTENDED_ID) {
        buf[msg_idx] -= 32U;
        id_len = SLCAN_EXTENDED_ID_BYTE_LENGTH;
    }

    msg_idx++;

    for (uint8_t i = id_len; i > 0; i--) {
        buf[i] = (id_tmp & 0xF);
        id_tmp >>= 4;
        msg_idx++;
    }

    buf[msg_idx++] = prv_convert_dlc_code_hal_to_standard(frame_header->DataLength);
    int8_t dlc_code_byte_length = hal_dlc_code_byte_length(frame_header->DataLength);

    if (dlc_code_byte_length < 0 || dlc_code_byte_length > 64)
        return -1;

    if (frame_header->RxFrameType != FDCAN_REMOTE_FRAME) {
        for (uint8_t i = 0; i < dlc_code_byte_length; i++) {
            buf[msg_idx++] = (frame_data[i] >> 4);
            buf[msg_idx++] = (frame_data[i] & 0x0F);
        }
    }

    for (uint8_t i = 1; i < msg_idx; i++) {
        if (buf[i] < 10) {
            buf[i] += '0';
        } else {
            buf[i] += ('a' - 10);
        }
    }

    buf[msg_idx++] = '\r';

    return msg_idx;
}

/*!
 * \brief Parses an incoming slcan frame from the USB CDC port and transmits it to the CAN queue
 *
 * \param[in] buf The buffer containing the slcan frame
 * \param[in] len The length of the buffer in bytes
 * \return 0 if success, -1 if any error happens
 */
int32_t slcan_parse_string(uint8_t itf, uint8_t *buf, uint8_t len) {
    FDCAN_TxHeaderTypeDef frame_header = {
        .TxFrameType = FDCAN_DATA_FRAME,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .IdType = FDCAN_STANDARD_ID,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0,
    };

    uint8_t frame_data[64] = { 0 };

    for (uint8_t i = 1; i < len; i++) {
        if (buf[i] >= 'a') {
            buf[i] = buf[i] - 'a' + 10;
        } else if (buf[i] >= 'A') {
            buf[i] = buf[i] - 'A' + 10;
        } else {
            buf[i] = buf[i] - '0';
        }
    }

    switch (buf[0]) {
        case 'O':
            fdcan_enable(itf);
            return 0;

        case 'C':
            fdcan_disable(itf);
            return 0;

        case 'S':
            if (buf[1] >= FDCAN_NOMINAL_BITRATE_INVALID) {
                return -1;
            }

            fdcan_set_nominal_bitrate(itf, buf[1]);
            return 0;

        case 'Y':
            if (buf[1] == 2) {
                fdcan_set_data_bitrate(itf, FDCAN_DATA_BITRATE_2M);
            } else if (buf[1] == 5) {
                fdcan_set_data_bitrate(itf, FDCAN_DATA_BITRATE_5M);
            } else {
                return -1;
            }

            return 0;

        case 'M':
            if (buf[1] == 1) {
                fdcan_set_silent(itf, true);
            } else {
                fdcan_set_silent(itf, false);
            }
            return 0;

        case 'A':
            if (buf[1] == 1) {
                fdcan_set_autoretransmit(itf, true);
            } else {
                fdcan_set_autoretransmit(itf, false);
            }
            return 0;

        case 'V': {
            char debug[32];
            snprintf(debug, sizeof(debug), "Interface #%d\r", itf);
            cdc_transmit(itf, (uint8_t *)debug, strlen(debug));
            return 0;
        }

        case 'T':
            frame_header.IdType = FDCAN_EXTENDED_ID;
            break;
        case 't':
            break;
        case 'r':
            frame_header.TxFrameType = FDCAN_REMOTE_FRAME;
            break;
        case 'R':
            frame_header.IdType = FDCAN_EXTENDED_ID;
            frame_header.TxFrameType = FDCAN_REMOTE_FRAME;
            break;
        case 'd':
            frame_header.FDFormat = FDCAN_FD_CAN;
            break;
        case 'D':
            frame_header.FDFormat = FDCAN_FD_CAN;
            frame_header.IdType = FDCAN_EXTENDED_ID;
            break;
        case 'b':
            frame_header.FDFormat = FDCAN_FD_CAN;
            frame_header.BitRateSwitch = FDCAN_BRS_ON;
            break;
        case 'B':
            frame_header.FDFormat = FDCAN_FD_CAN;
            frame_header.BitRateSwitch = FDCAN_BRS_ON;
            frame_header.IdType = FDCAN_EXTENDED_ID;
            break;
        default:
            return -1;
    }

    uint8_t parse_loc = 1;
    frame_header.Identifier = 0;
    uint8_t id_len = SLCAN_STANDARD_ID_BYTE_LENGTH;

    if (frame_header.IdType == FDCAN_EXTENDED_ID)
        id_len = SLCAN_EXTENDED_ID_BYTE_LENGTH;

    while (parse_loc <= id_len) {
        frame_header.Identifier *= 16;
        frame_header.Identifier += buf[parse_loc++];
    }

    uint8_t dlc_code_raw = buf[parse_loc++];

    if (frame_header.FDFormat == FDCAN_FD_CAN && dlc_code_raw > 0xF) {
        return -1;
    }
    if (frame_header.FDFormat == FDCAN_CLASSIC_CAN && dlc_code_raw > 0x8) {
        return -1;
    }

    frame_header.DataLength = prv_convert_dlc_code_standard_to_hal(dlc_code_raw);

    int8_t bytes_in_msg = hal_dlc_code_byte_length(frame_header.DataLength);

    if (bytes_in_msg < 0 || bytes_in_msg > 64) {
        return -1;
    }

    for (uint8_t i = 0; i < bytes_in_msg; i++) {
        frame_data[i] = (buf[parse_loc] << 4) + buf[parse_loc + 1];
        parse_loc += 2;
    }

    fdcan_transmit(itf, &frame_header, frame_data);

    return 0;
}
