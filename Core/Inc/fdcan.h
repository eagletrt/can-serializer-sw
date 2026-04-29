/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    fdcan.h
 * @brief   This file contains all the function prototypes for
 *          the fdcan.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include <stdbool.h>
#include <stdint.h>

/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

extern FDCAN_HandleTypeDef hfdcan2;

extern FDCAN_HandleTypeDef hfdcan3;

/* USER CODE BEGIN Private defines */

/*!
 * \brief An enumeration listing the available interfaces, one per FDCAN peripheral
 */
enum fdcan_itf {
    FDCAN_ITF_FDCAN1,
    FDCAN_ITF_FDCAN2,
    FDCAN_ITF_FDCAN3,
    FDCAN_ITF_COUNT
};

/*!
 * \brief An enumeration listing the available nominal bitrates of an FDCAN peripheral
 */
enum fdcan_nominal_bitrate {
    FDCAN_NOMINAL_BITRATE_10K = 0,
    FDCAN_NOMINAL_BITRATE_20K,
    FDCAN_NOMINAL_BITRATE_50K,
    FDCAN_NOMINAL_BITRATE_100K,
    FDCAN_NOMINAL_BITRATE_125K,
    FDCAN_NOMINAL_BITRATE_250K,
    FDCAN_NOMINAL_BITRATE_500K,
    FDCAN_NOMINAL_BITRATE_750K,
    FDCAN_NOMINAL_BITRATE_1000K,
    FDCAN_NOMINAL_BITRATE_83_3K,

    FDCAN_NOMINAL_BITRATE_INVALID,
};

/*!
 * \brief An enumeration listing the available data bitrates of an FDCAN peripheral
 */
enum fdcan_data_bitrate {
    FDCAN_DATA_BITRATE_2M = 2,
    FDCAN_DATA_BITRATE_5M = 5,

    FDCAN_DATA_BITRATE_INVALID,
};

/*!
 * \brief An enumeration listing the possible states of the FDCAN bus
 */
enum fdcan_bus_state { OFF_BUS,
                       ON_BUS };

#define FDCAN_TX_QUEUE_LENGTH (64U)      // Number of buffers allocated
#define FDCAN_TX_QUEUE_DATA_LENGTH (64U) // CAN DLC length of data buffers, must be 64 for CAN FD.

/*!
 * \brief A structure describing a circular buffer used for the frames that need to be tramsitted
 */
struct fdcan_tx_buf {
    uint8_t data[FDCAN_TX_QUEUE_LENGTH][FDCAN_TX_QUEUE_DATA_LENGTH]; // Data buffer
    FDCAN_TxHeaderTypeDef header[FDCAN_TX_QUEUE_LENGTH];             // Header buffer
    uint16_t head;                                                   // Head pointer
    uint16_t tail;                                                   // Tail pointer
    uint8_t full;                                                    // TODO: Set this when we are full, clear when the tail moves one.
};

/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);
void MX_FDCAN2_Init(void);
void MX_FDCAN3_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief Enables one of the available FDCAN peripherals
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 */
void fdcan_enable(enum fdcan_itf itf);

/*!
 * \brief Disables one of the available FDCAN peripherals
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 */
void fdcan_disable(enum fdcan_itf itf);

/*!
 * \brief Sets the FDCAN nominal bitrate by adjusting the prescaler value
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] nominal_bitrate An fdcan_nominal_bitrate enumeration type value corresponding to a specific nominal bitrate
 */
void fdcan_set_nominal_bitrate(enum fdcan_itf itf, enum fdcan_nominal_bitrate nominal_bitrate);

/*!
 * \brief Sets the FDCAN data bitrate by adjusting the prescaler value
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] data_bitrate An fdcan_data_bitrate enumeration type value corresponding to a specific data bitrate
 */
void fdcan_set_data_bitrate(enum fdcan_itf itf, enum fdcan_data_bitrate data_bitrate);

/*!
 * \brief Sets one of the available FDCAN peripherals to silent mode
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] silent A boolean indicating whether or not to turn on silent mode (bus monitoring)
 */
void fdcan_set_silent(enum fdcan_itf itf, bool silent);

/*!
 * \brief Set one of the available FDCAN peripherals to autoretransmission mode
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] autoretransmit A boolean indicating whether or not to turn on autoretransmission
 */
void fdcan_set_autoretransmit(enum fdcan_itf itf, bool autoretransmit);

/*!
 * \brief Handles the transmission of a message on the FDCAN bus
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] tx_msg_header A pointer to an FDCAN_TxHeaderTypeDef struct containing the description of the message
 * \param[in] tx_msg_data A pointer to the message data
 * \return HAL_ERROR on error (related to queue size), HAL_OK on success
 */
uint32_t fdcan_transmit(enum fdcan_itf itf, FDCAN_TxHeaderTypeDef *tx_msg_header, uint8_t *tx_msg_data);

/*!
 * \brief Handles the reception of a message from the FDCAN bus
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] rx_msg_header A pointer to an FDCAN_RxHeaderTypeDef structure describing the structure of the message
 * \param[in] rx_msg_data A pointer to the message data
 * \return The status of the operation (HAL_OK on success, HAL_ERROR on failure)
 */
uint32_t fdcan_receive(enum fdcan_itf itf, FDCAN_RxHeaderTypeDef *rx_msg_header, uint8_t *rx_msg_data);

/*!
 * \brief Handles the processing of the data from the FDCAN circular buffers
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 */
void fdcan_process(void);

/*!
 * \brief Checks if a message has been received and is waiting in the queue
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] fifo An integer that corresponds to the location of the FIFO data structure
 * \return A value equivalent to true if a message is pending, one equivalent to false otherwise
 */
bool is_fdcan_msg_pending(enum fdcan_itf itf, uint8_t fifo);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */
