/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    fdcan.c
 * @brief   This file provides code for the configuration
 * of the FDCAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "fdcan.h"
#include "eagletrt.h"
#include "slcan.h"
#include "gpio.h"
#include "stm32h7xx_hal_fdcan.h"

/* USER CODE BEGIN 0 */

EAGLETRT_STATIC uint32_t nominal_prescaler = 24;
EAGLETRT_STATIC uint32_t data_prescaler = 3;
enum fdcan_bus_state bus_state = OFF_BUS;
EAGLETRT_STATIC uint8_t fdcan_autoretransmit = ENABLE;
EAGLETRT_STATIC struct fdcan_tx_buf tx_queue = { 0 };

/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void) {

    /* USER CODE BEGIN FDCAN1_Init 0 */

    /* USER CODE END FDCAN1_Init 0 */

    /* USER CODE BEGIN FDCAN1_Init 1 */

    /* USER CODE END FDCAN1_Init 1 */
    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    hfdcan1.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
    hfdcan1.Init.AutoRetransmission = DISABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;
    hfdcan1.Init.NominalPrescaler = 24;
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1 = 15;
    hfdcan1.Init.NominalTimeSeg2 = 4;
    hfdcan1.Init.DataPrescaler = 3;
    hfdcan1.Init.DataSyncJumpWidth = 1;
    hfdcan1.Init.DataTimeSeg1 = 7;
    hfdcan1.Init.DataTimeSeg2 = 2;
    hfdcan1.Init.MessageRAMOffset = 0;
    hfdcan1.Init.StdFiltersNbr = 0;
    hfdcan1.Init.ExtFiltersNbr = 0;
    hfdcan1.Init.RxFifo0ElmtsNbr = 16;
    hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
    hfdcan1.Init.RxFifo1ElmtsNbr = 0;
    hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.RxBuffersNbr = 0;
    hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.TxEventsNbr = 0;
    hfdcan1.Init.TxBuffersNbr = 0;
    hfdcan1.Init.TxFifoQueueElmtsNbr = 16;
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_64;
    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN FDCAN1_Init 2 */

    /* USER CODE END FDCAN1_Init 2 */
}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *fdcanHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };
    if (fdcanHandle->Instance == FDCAN1) {
        /* USER CODE BEGIN FDCAN1_MspInit 0 */

        /* USER CODE END FDCAN1_MspInit 0 */

        /** Initializes the peripherals clock
  */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
            Error_Handler();
        }

        /* FDCAN1 clock enable */
        __HAL_RCC_FDCAN_CLK_ENABLE();

        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        /* USER CODE BEGIN FDCAN1_MspInit 1 */

        fdcan_set_nominal_bitrate(FDCAN_NOMINAL_BITRATE_125K);
        fdcan_set_data_bitrate(FDCAN_DATA_BITRATE_2M);
        hfdcan1.Instance = FDCAN1;

        /* USER CODE END FDCAN1_MspInit 1 */
    }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *fdcanHandle) {

    if (fdcanHandle->Instance == FDCAN1) {
        /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

        /* USER CODE END FDCAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_FDCAN_CLK_DISABLE();

        /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1);

        /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

        /* USER CODE END FDCAN1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/*!
 * \brief Enables the FDCAN peripheral
 */
void fdcan_enable(void) {
    if (bus_state == OFF_BUS) {
        hfdcan1.Init.NominalPrescaler = nominal_prescaler;
        hfdcan1.Init.AutoRetransmission = fdcan_autoretransmit;

        if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
            Error_Handler();
        }

        // Filters are not needed (at least for now).
        /*
        HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                     FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_FILTER_REMOTE,
                                     FDCAN_FILTER_REMOTE);
        */

        HAL_FDCAN_Start(&hfdcan1);
        bus_state = ON_BUS;

        led_yellow_on();
    }
}

/*!
 * \brief Disables the FDCAN peripheral
 */
void fdcan_disable(void) {
    if (bus_state == ON_BUS) {
        HAL_FDCAN_Stop(&hfdcan1);
        bus_state = OFF_BUS;
        led_green_on();
    }
}

/*!
 * \brief Sets the FDCAN data bitrate by adjusting the prescaler value
 *
 * \param[in] data_bitrate An fdcan_data_bitrate enumeration type value corresponding to a specific data bitrate
 */
void fdcan_set_data_bitrate(enum fdcan_data_bitrate data_bitrate) {
    /*
     * The default is 2 Mbit, which is 
     */

    if (bus_state != ON_BUS) {
        hfdcan1.Init.DataSyncJumpWidth = 2;
        hfdcan1.Init.DataTimeSeg1 = 7; // (Sync + Seg1) = 8
        hfdcan1.Init.DataTimeSeg2 = 2; // Total TQ = 10

        switch (data_bitrate) {
            case FDCAN_DATA_BITRATE_2M:
                hfdcan1.Init.DataPrescaler = 3;
                break;

            case FDCAN_DATA_BITRATE_5M:
                hfdcan1.Init.DataPrescaler = 1;
                hfdcan1.Init.DataTimeSeg1 = 9;
                hfdcan1.Init.DataTimeSeg2 = 2;
                hfdcan1.Init.DataSyncJumpWidth = 2;
                break;

            default:
                hfdcan1.Init.DataPrescaler = 3;
                break;
        }
    }

    led_green_on();
}

/*!
 * @brief Sets the FDCAN nominal bitrate by adjusting the prescaler value
 *
 * @param[in] nominal_bitrate An fdcan_nominal_bitrate enumeration type value corresponding to a specific nominal bitrate
 */
void fdcan_set_nominal_bitrate(enum fdcan_nominal_bitrate nominal_bitrate) {
    if (bus_state != ON_BUS) {

        switch (nominal_bitrate) {
            case FDCAN_NOMINAL_BITRATE_10K:
                nominal_prescaler = 300;
                break;
            case FDCAN_NOMINAL_BITRATE_20K:
                nominal_prescaler = 150;
                break;
            case FDCAN_NOMINAL_BITRATE_50K:
                nominal_prescaler = 60;
                break;
            case FDCAN_NOMINAL_BITRATE_83_3K:
                nominal_prescaler = 36;
                break;
            case FDCAN_NOMINAL_BITRATE_100K:
                nominal_prescaler = 30;
                break;
            case FDCAN_NOMINAL_BITRATE_125K:
                nominal_prescaler = 24;
                break;
            case FDCAN_NOMINAL_BITRATE_250K:
                nominal_prescaler = 12;
                break;
            case FDCAN_NOMINAL_BITRATE_500K:
                nominal_prescaler = 6;
                break;
            case FDCAN_NOMINAL_BITRATE_750K:
                nominal_prescaler = 4;
                break;
            case FDCAN_NOMINAL_BITRATE_1000K:
            default:
                nominal_prescaler = 3;
                break;
        }
    }

    led_green_on();
}

/*!
 * \brief Sets the FDCAN peripheral to silent mode
 *
 * \param[in] silent FIXME: An integer value corresponding to a boolean
 */
void fdcan_set_silent(uint8_t silent) {
    if (bus_state == ON_BUS) {
        // cannot set silent mode while on bus
        return;
    }
    if (silent) {
        hfdcan1.Init.Mode = FDCAN_MODE_BUS_MONITORING;
    } else {
        // NOTE: Should default to FDCAN_MODE_NORMAL here instead of loopback when not testing
        hfdcan1.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
    }

    led_green_on();
}

// Set CAN peripheral to autoretransmission mode

/*!
 * \brief Set the FDCAN peripheral to autoretransmission mode
 *
 * \param[in] autoretransmit FIXME: An integer value corresponding to a boolean
 */
void fdcan_set_autoretransmit(uint8_t autoretransmit) {
    if (bus_state == ON_BUS) {
        // Cannot set autoretransmission while on bus
        return;
    }
    if (autoretransmit) {
        fdcan_autoretransmit = ENABLE;
    } else {
        fdcan_autoretransmit = DISABLE;
    }

    led_green_on();
}

/*!
 * \brief Sends a message on the FDCAN bus
 *
 * \param[in] tx_msg_header A pointer to an FDCAN_TxHeaderTypeDef structure describing the structure of the message
 * \param[in] tx_msg_data A pointer to the message data
 * \return HAL_ERROR on error (related to queue size), HAL_OK on success
 */
uint32_t fdcan_transmit(FDCAN_TxHeaderTypeDef *tx_msg_header, uint8_t *tx_msg_data) {
    // If when we increment the head we're going to hit the tail
    // (if we're filling the last spot in the queue)
    if (((tx_queue.head + 1) % FDCAN_TX_QUEUE_LENGTH) == tx_queue.tail) {
        return HAL_ERROR;
    }

    // Convert length to bytes
    uint32_t len = hal_dlc_code_byte_length(tx_msg_header->DataLength);

    // Don't overrun buffer element max length
    if (len > FDCAN_TX_QUEUE_DATA_LENGTH)
        return HAL_ERROR;

    tx_queue.header[tx_queue.head] = *tx_msg_header;

    for (uint8_t i = 0; i < len; i++) {
        tx_queue.data[tx_queue.head][i] = tx_msg_data[i];
    }

    // Increment the head pointer
    tx_queue.head = (tx_queue.head + 1) % FDCAN_TX_QUEUE_LENGTH;

    return HAL_OK;
}

/*!
 * \brief Processes the data from the FDCAN circular buffers
 */
void fdcan_process(void) {
    while ((tx_queue.tail != tx_queue.head) && (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0)) {
        // uint32_t status;
        HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_queue.header[tx_queue.tail], tx_queue.data[tx_queue.tail]);
        tx_queue.tail = (tx_queue.tail + 1) % FDCAN_TX_QUEUE_LENGTH;

        led_green_on();
    }
}

/*!
 * \brief Receives a message from the FDCAN bus
 *
 * \param[in] rx_msg_header A pointer to an FDCAN_RxHeaderTypeDef structure describing the structure of the message
 * \param[in] rx_msg_data A pointer to the message data
 * \return The status of the operation (HAL_OK on success, HAL_ERROR on failure)
 */
uint32_t fdcan_receive(FDCAN_RxHeaderTypeDef *rx_msg_header, uint8_t *rx_msg_data) {
    uint32_t status;
    status = HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, rx_msg_header, rx_msg_data);

    led_yellow_on();
    return status;
}

/*!
 * \brief Checks if a message has been received and is waiting in the queue
 *
 * \param[in] fifo An integer referencing the location of the FIFO
 * \return 1 if a message is pending, 0 otherwise
 */
uint8_t is_fdcan_msg_pending(uint8_t fifo) {
    if (bus_state == OFF_BUS) {
        return 0;
    }

    return (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, fifo) > 0);
}

/*!
 * \brief Gets the handle to the FDCAN peripheral
 *
 * \return The handle to the FDCAN peripheral
 */
FDCAN_HandleTypeDef *fdcan_gethandle(void) {
    return &hfdcan1;
}

/* USER CODE END 1 */
