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

/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

/* USER CODE BEGIN Private defines */

// Classic CAN / CANFD nominal bitrates
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

// CANFD bitrates
enum fdcan_data_bitrate {
    FDCAN_DATA_BITRATE_2M = 2,
    FDCAN_DATA_BITRATE_5M = 5,

    FDCAN_DATA_BITRATE_INVALID,
};

// Bus state
enum fdcan_bus_state { OFF_BUS,
                       ON_BUS };

// CAN transmit buffering
#define FDCAN_TX_QUEUE_LENGTH (64U)      // Number of buffers allocated
#define FDCAN_TX_QUEUE_DATA_LENGTH (64U) // CAN DLC length of data buffers, must be 64 for CAN FD.

// Cirbuf structure for CAN TX frames
struct fdcan_tx_buf {
    uint8_t data[FDCAN_TX_QUEUE_LENGTH][FDCAN_TX_QUEUE_DATA_LENGTH]; // Data buffer
    FDCAN_TxHeaderTypeDef header[FDCAN_TX_QUEUE_LENGTH];             // Header buffer
    uint16_t head;                                                   // Head pointer
    uint16_t tail;                                                   // Tail pointer
    uint8_t full;                                                    // TODO: Set this when we are full, clear when the tail moves one.
};

/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);

/* USER CODE BEGIN Prototypes */

void fdcan_enable(void);
void fdcan_disable(void);
void fdcan_set_nominal_bitrate(enum fdcan_nominal_bitrate nominal_bitrate);
void fdcan_set_data_bitrate(enum fdcan_data_bitrate data_bitrate);
void fdcan_set_silent(uint8_t silent);
void fdcan_set_autoretransmit(uint8_t autoretransmit);
uint32_t fdcan_transmit(FDCAN_TxHeaderTypeDef *tx_msg_header, uint8_t *tx_msg_data);
uint32_t fdcan_receive(FDCAN_RxHeaderTypeDef *rx_msg_header, uint8_t *rx_msg_data);
void fdcan_process(void);
uint8_t is_fdcan_msg_pending(uint8_t fifo);
FDCAN_HandleTypeDef *fdcan_get_handle(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */
