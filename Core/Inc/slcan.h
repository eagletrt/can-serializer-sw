#ifndef SLCAN_H
#define SLCAN_H

#include <stdint.h>
#include "fdcan.h"

// Since the maximum length of a CAN FD frame is 64 bytes and we are using hexadecimal to encode it, it must be that the maximum length of an slcan frame is 64 * 2 bytes
#define SLCAN_FDCAN_MAX_BYTE_LENGTH_DATA (128U)

// In an slcan frame, the type is encoded by a single character
#define SLCAN_TYPE_BYTE_LENGTH (1U)

// In an slcan frame, the the data length code is encoded by a single character
#define SLCAN_DLC_BYTE_LENGTH (1U)

// A valid standard ID in an slcan frame is encoded using hexadecimal values between 0 and 7FF (11 bits), stored as characters
#define SLCAN_STANDARD_ID_BYTE_LENGTH (3U)

// A valid extended ID in an slcan frame is encoded using hexadecimal values between 0 and 1FFFFFFF (29 bits), stored as charaters
#define SLCAN_EXTENDED_ID_BYTE_LENGTH (8U)

// A valid slcan frame is terminated by a carriage return (\r)
#define SLCAN_TERMINATOR_BYTE_LENGTH (1U)

// The total maximum length of an slcan frame is the sum of the maximum lengths of each of its components plus some overhead
#define SLCAN_MAXIMUM_TRANSMISSION_UNIT (SLCAN_TYPE_BYTE_LENGTH + SLCAN_EXTENDED_ID_BYTE_LENGTH + SLCAN_DLC_BYTE_LENGTH + SLCAN_FDCAN_MAX_BYTE_LENGTH_DATA + SLCAN_TERMINATOR_BYTE_LENGTH + 11U)

/*!
 * \brief Converts an FDCAN_DLC_BYTES value to the number of bytes in a message
 *
 * \param[in] hal_dlc_code An HAL defined data length code (FDCAN_DLC_BYTES)
 * \return A value corresponding to the number of bytes associated to a data length code
 */
int8_t hal_dlc_code_byte_length(uint32_t hal_dlc_code);

/*!
 * \brief Parses an incoming CAN frame into an outgoing slcan frame (as an ASCII string)
 *
 * \param[out] buf The buffer containing the slcan frame
 * \param[in] frame_header The CAN frame header containing its description
 * \param[in] frame_data The CAN frame data
 * \return The slcan frame length in bytes
 */
int32_t slcan_parse_frame(uint8_t *buf, FDCAN_RxHeaderTypeDef *frame_header, uint8_t *frame_data);

/*!
 * \brief Parses an incoming slcan frame from the USB CDC port and transmits it to the CAN queue
 *
 * \param[in] buf The buffer containing the slcan frame
 * \param[in] len The length of the buffer in bytes
 * \return 0 if success, -1 if any error happens
 */
int32_t slcan_parse_string(uint8_t itf, uint8_t *buf, uint8_t len);

#endif
