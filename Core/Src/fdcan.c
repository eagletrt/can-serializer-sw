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
#include "slcan.h"
#include "gpio.h"
#include "eagletrt.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal_fdcan.h"

/* USER CODE BEGIN 0 */

EAGLETRT_STATIC uint32_t nominal_prescaler[FDCAN_ITF_COUNT] = { 24, 24, 24 };
enum fdcan_bus_state bus_state[FDCAN_ITF_COUNT] = { OFF_BUS, OFF_BUS, OFF_BUS };
EAGLETRT_STATIC uint8_t fdcan_autoretransmit[FDCAN_ITF_COUNT] = { ENABLE, ENABLE, ENABLE };
EAGLETRT_STATIC struct fdcan_tx_buf tx_queue[FDCAN_ITF_COUNT] = { 0 };

/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;
FDCAN_HandleTypeDef hfdcan2;
FDCAN_HandleTypeDef hfdcan3;

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
/* FDCAN2 init function */
void MX_FDCAN2_Init(void) {

    /* USER CODE BEGIN FDCAN2_Init 0 */

    /* USER CODE END FDCAN2_Init 0 */

    /* USER CODE BEGIN FDCAN2_Init 1 */

    /* USER CODE END FDCAN2_Init 1 */
    hfdcan2.Instance = FDCAN2;
    hfdcan2.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    hfdcan2.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
    hfdcan2.Init.AutoRetransmission = DISABLE;
    hfdcan2.Init.TransmitPause = DISABLE;
    hfdcan2.Init.ProtocolException = DISABLE;
    hfdcan2.Init.NominalPrescaler = 24;
    hfdcan2.Init.NominalSyncJumpWidth = 1;
    hfdcan2.Init.NominalTimeSeg1 = 15;
    hfdcan2.Init.NominalTimeSeg2 = 4;
    hfdcan2.Init.DataPrescaler = 3;
    hfdcan2.Init.DataSyncJumpWidth = 1;
    hfdcan2.Init.DataTimeSeg1 = 7;
    hfdcan2.Init.DataTimeSeg2 = 2;
    hfdcan2.Init.MessageRAMOffset = 840;
    hfdcan2.Init.StdFiltersNbr = 0;
    hfdcan2.Init.ExtFiltersNbr = 0;
    hfdcan2.Init.RxFifo0ElmtsNbr = 16;
    hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
    hfdcan2.Init.RxFifo1ElmtsNbr = 0;
    hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan2.Init.RxBuffersNbr = 0;
    hfdcan2.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
    hfdcan2.Init.TxEventsNbr = 0;
    hfdcan2.Init.TxBuffersNbr = 0;
    hfdcan2.Init.TxFifoQueueElmtsNbr = 16;
    hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    hfdcan2.Init.TxElmtSize = FDCAN_DATA_BYTES_64;
    if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN FDCAN2_Init 2 */

    /* USER CODE END FDCAN2_Init 2 */
}
/* FDCAN3 init function */
void MX_FDCAN3_Init(void) {

    /* USER CODE BEGIN FDCAN3_Init 0 */

    /* USER CODE END FDCAN3_Init 0 */

    /* USER CODE BEGIN FDCAN3_Init 1 */

    /* USER CODE END FDCAN3_Init 1 */
    hfdcan3.Instance = FDCAN3;
    hfdcan3.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    hfdcan3.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
    hfdcan3.Init.AutoRetransmission = DISABLE;
    hfdcan3.Init.TransmitPause = DISABLE;
    hfdcan3.Init.ProtocolException = DISABLE;
    hfdcan3.Init.NominalPrescaler = 24;
    hfdcan3.Init.NominalSyncJumpWidth = 1;
    hfdcan3.Init.NominalTimeSeg1 = 15;
    hfdcan3.Init.NominalTimeSeg2 = 4;
    hfdcan3.Init.DataPrescaler = 3;
    hfdcan3.Init.DataSyncJumpWidth = 1;
    hfdcan3.Init.DataTimeSeg1 = 7;
    hfdcan3.Init.DataTimeSeg2 = 2;
    hfdcan3.Init.MessageRAMOffset = 1680;
    hfdcan3.Init.StdFiltersNbr = 0;
    hfdcan3.Init.ExtFiltersNbr = 0;
    hfdcan3.Init.RxFifo0ElmtsNbr = 16;
    hfdcan3.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
    hfdcan3.Init.RxFifo1ElmtsNbr = 0;
    hfdcan3.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan3.Init.RxBuffersNbr = 0;
    hfdcan3.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
    hfdcan3.Init.TxEventsNbr = 0;
    hfdcan3.Init.TxBuffersNbr = 0;
    hfdcan3.Init.TxFifoQueueElmtsNbr = 16;
    hfdcan3.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    hfdcan3.Init.TxElmtSize = FDCAN_DATA_BYTES_64;
    if (HAL_FDCAN_Init(&hfdcan3) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN FDCAN3_Init 2 */

    /* USER CODE END FDCAN3_Init 2 */
}

static uint32_t HAL_RCC_FDCAN_CLK_ENABLED = 0;

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
        HAL_RCC_FDCAN_CLK_ENABLED++;
        if (HAL_RCC_FDCAN_CLK_ENABLED == 1) {
            __HAL_RCC_FDCAN_CLK_ENABLE();
        }

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

        fdcan_set_nominal_bitrate(FDCAN_ITF_FDCAN1, FDCAN_NOMINAL_BITRATE_125K);
        fdcan_set_data_bitrate(FDCAN_ITF_FDCAN1, FDCAN_DATA_BITRATE_2M);
        hfdcan1.Instance = FDCAN1;

        /* USER CODE END FDCAN1_MspInit 1 */
    } else if (fdcanHandle->Instance == FDCAN2) {
        /* USER CODE BEGIN FDCAN2_MspInit 0 */

        /* USER CODE END FDCAN2_MspInit 0 */

        /** Initializes the peripherals clock
  */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
            Error_Handler();
        }

        /* FDCAN2 clock enable */
        HAL_RCC_FDCAN_CLK_ENABLED++;
        if (HAL_RCC_FDCAN_CLK_ENABLED == 1) {
            __HAL_RCC_FDCAN_CLK_ENABLE();
        }

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**FDCAN2 GPIO Configuration
    PB12     ------> FDCAN2_RX
    PB13     ------> FDCAN2_TX
    */
        GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN FDCAN2_MspInit 1 */

        fdcan_set_nominal_bitrate(FDCAN_ITF_FDCAN2, FDCAN_NOMINAL_BITRATE_125K);
        fdcan_set_data_bitrate(FDCAN_ITF_FDCAN2, FDCAN_DATA_BITRATE_2M);
        hfdcan2.Instance = FDCAN2;

        /* USER CODE END FDCAN2_MspInit 1 */
    } else if (fdcanHandle->Instance == FDCAN3) {
        /* USER CODE BEGIN FDCAN3_MspInit 0 */

        /* USER CODE END FDCAN3_MspInit 0 */

        /** Initializes the peripherals clock
  */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
            Error_Handler();
        }

        /* FDCAN3 clock enable */
        HAL_RCC_FDCAN_CLK_ENABLED++;
        if (HAL_RCC_FDCAN_CLK_ENABLED == 1) {
            __HAL_RCC_FDCAN_CLK_ENABLE();
        }

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**FDCAN3 GPIO Configuration
    PF6     ------> FDCAN3_RX
    PF7     ------> FDCAN3_TX
    */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_FDCAN3;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* USER CODE BEGIN FDCAN3_MspInit 1 */

        fdcan_set_nominal_bitrate(FDCAN_ITF_FDCAN3, FDCAN_NOMINAL_BITRATE_125K);
        fdcan_set_data_bitrate(FDCAN_ITF_FDCAN3, FDCAN_DATA_BITRATE_2M);
        hfdcan3.Instance = FDCAN3;

        /* USER CODE END FDCAN3_MspInit 1 */
    }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *fdcanHandle) {

    if (fdcanHandle->Instance == FDCAN1) {
        /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

        /* USER CODE END FDCAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        HAL_RCC_FDCAN_CLK_ENABLED--;
        if (HAL_RCC_FDCAN_CLK_ENABLED == 0) {
            __HAL_RCC_FDCAN_CLK_DISABLE();
        }

        /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1);

        /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

        /* USER CODE END FDCAN1_MspDeInit 1 */
    } else if (fdcanHandle->Instance == FDCAN2) {
        /* USER CODE BEGIN FDCAN2_MspDeInit 0 */

        /* USER CODE END FDCAN2_MspDeInit 0 */
        /* Peripheral clock disable */
        HAL_RCC_FDCAN_CLK_ENABLED--;
        if (HAL_RCC_FDCAN_CLK_ENABLED == 0) {
            __HAL_RCC_FDCAN_CLK_DISABLE();
        }

        /**FDCAN2 GPIO Configuration
    PB12     ------> FDCAN2_RX
    PB13     ------> FDCAN2_TX
    */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12 | GPIO_PIN_13);

        /* USER CODE BEGIN FDCAN2_MspDeInit 1 */

        /* USER CODE END FDCAN2_MspDeInit 1 */
    } else if (fdcanHandle->Instance == FDCAN3) {
        /* USER CODE BEGIN FDCAN3_MspDeInit 0 */

        /* USER CODE END FDCAN3_MspDeInit 0 */
        /* Peripheral clock disable */
        HAL_RCC_FDCAN_CLK_ENABLED--;
        if (HAL_RCC_FDCAN_CLK_ENABLED == 0) {
            __HAL_RCC_FDCAN_CLK_DISABLE();
        }

        /**FDCAN3 GPIO Configuration
    PF6     ------> FDCAN3_RX
    PF7     ------> FDCAN3_TX
    */
        HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6 | GPIO_PIN_7);

        /* USER CODE BEGIN FDCAN3_MspDeInit 1 */

        /* USER CODE END FDCAN3_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/*!
 * \brief Gets the handle to one of the available FDCAN peripherals
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \return The handle to one of the available FDCAN peripherals
 */
EAGLETRT_STATIC FDCAN_HandleTypeDef *fdcan_get_handle_by_itf(enum fdcan_itf itf) {
    switch (itf) {
        case FDCAN_ITF_FDCAN1:
            return &hfdcan1;
        case FDCAN_ITF_FDCAN2:
            return &hfdcan2;
        case FDCAN_ITF_FDCAN3:
            return &hfdcan3;
        default:
            return &hfdcan1;
    }
}

/*!
 * \brief Enables one of the available FDCAN peripherals
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 */
void fdcan_enable(enum fdcan_itf itf) {
    FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

    if (bus_state[itf] == OFF_BUS) {
        hfdcan->Init.NominalPrescaler = nominal_prescaler[itf];
        hfdcan->Init.AutoRetransmission = fdcan_autoretransmit[itf];

        if (HAL_FDCAN_Init(hfdcan) != HAL_OK) {
            Error_Handler();
        }

        HAL_FDCAN_Start(hfdcan);
        bus_state[itf] = ON_BUS;

        led_yellow_on();
    }
}

/*!
 * \brief Disables one of the available FDCAN peripherals
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 */
void fdcan_disable(enum fdcan_itf itf) {
    FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

    if (bus_state[itf] == ON_BUS) {
        HAL_FDCAN_Stop(hfdcan);
        bus_state[itf] = OFF_BUS;
        led_green_on();
    }
}

/*!
 * \brief Sets the FDCAN nominal bitrate by adjusting the prescaler value
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] nominal_bitrate An fdcan_nominal_bitrate enumeration type value corresponding to a specific nominal bitrate
 */
void fdcan_set_nominal_bitrate(enum fdcan_itf itf, enum fdcan_nominal_bitrate nominal_bitrate) {
    if (bus_state[itf] != ON_BUS) {

        switch (nominal_bitrate) {
            case FDCAN_NOMINAL_BITRATE_10K:
                nominal_prescaler[itf] = 300;
                break;
            case FDCAN_NOMINAL_BITRATE_20K:
                nominal_prescaler[itf] = 150;
                break;
            case FDCAN_NOMINAL_BITRATE_50K:
                nominal_prescaler[itf] = 60;
                break;
            case FDCAN_NOMINAL_BITRATE_83_3K:
                nominal_prescaler[itf] = 36;
                break;
            case FDCAN_NOMINAL_BITRATE_100K:
                nominal_prescaler[itf] = 30;
                break;
            case FDCAN_NOMINAL_BITRATE_125K:
                nominal_prescaler[itf] = 24;
                break;
            case FDCAN_NOMINAL_BITRATE_250K:
                nominal_prescaler[itf] = 12;
                break;
            case FDCAN_NOMINAL_BITRATE_500K:
                nominal_prescaler[itf] = 6;
                break;
            case FDCAN_NOMINAL_BITRATE_750K:
                nominal_prescaler[itf] = 4;
                break;
            case FDCAN_NOMINAL_BITRATE_1000K:
            default:
                nominal_prescaler[itf] = 3;
                break;
        }
    }

    led_green_on();
}

/*!
 * \brief Sets the FDCAN data bitrate by adjusting the prescaler value
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] data_bitrate An fdcan_data_bitrate enumeration type value corresponding to a specific data bitrate
 */
void fdcan_set_data_bitrate(enum fdcan_itf itf, enum fdcan_data_bitrate data_bitrate) {
    // FIXME: Since we don't plan to use bitrates higher than 1 mbit, this should be changed. This also means that BRS is not needed.

    FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

    if (bus_state[itf] != ON_BUS) {
        hfdcan->Init.DataSyncJumpWidth = 2;
        hfdcan->Init.DataTimeSeg1 = 7; // (Sync + Seg1) = 8
        hfdcan->Init.DataTimeSeg2 = 2; // Total TQ = 10

        switch (data_bitrate) {
            case FDCAN_DATA_BITRATE_2M:
                hfdcan->Init.DataPrescaler = 3;
                break;

            case FDCAN_DATA_BITRATE_5M:
                hfdcan->Init.DataPrescaler = 1;
                hfdcan->Init.DataTimeSeg1 = 9;
                hfdcan->Init.DataTimeSeg2 = 2;
                hfdcan->Init.DataSyncJumpWidth = 2;
                break;

            default:
                hfdcan->Init.DataPrescaler = 3;
                break;
        }
    }

    led_green_on();
}

/*!
 * \brief Sets one of the available FDCAN peripherals to silent mode
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] silent A boolean indicating whether or not to turn on silent mode (bus monitoring)
 */
void fdcan_set_silent(enum fdcan_itf itf, bool silent) {
    FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

    // cannot set silent mode while on bus
    if (bus_state[itf] != ON_BUS) {
        // NOTE: Should default to FDCAN_MODE_NORMAL here instead of loopback when not testing
        hfdcan->Init.Mode = (silent) ? FDCAN_MODE_BUS_MONITORING : FDCAN_MODE_INTERNAL_LOOPBACK;
    }

    led_green_on();
}

/*!
 * \brief Set one of the available FDCAN peripherals to autoretransmission mode
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] autoretransmit A boolean indicating whether or not to turn on autoretransmission
 */
void fdcan_set_autoretransmit(enum fdcan_itf itf, bool autoretransmit) {
    // Cannot set autoretransmission while on bus
    if (bus_state[itf] != ON_BUS) {
        fdcan_autoretransmit[itf] = (autoretransmit) ? ENABLE : DISABLE;
    }

    led_green_on();
}

/*!
 * \brief Handles the transmission of a message on the FDCAN bus
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] tx_msg_header A pointer to an FDCAN_TxHeaderTypeDef struct containing the description of the message
 * \param[in] tx_msg_data A pointer to the message data
 * \return HAL_ERROR on error (related to queue size), HAL_OK on success
 */
uint32_t fdcan_transmit(enum fdcan_itf itf, FDCAN_TxHeaderTypeDef *tx_msg_header, uint8_t *tx_msg_data) {
    // If when we increment the head we're going to hit the tail
    // (if we're filling the last spot in the queue)
    if (((tx_queue[itf].head + 1) % FDCAN_TX_QUEUE_LENGTH) == tx_queue[itf].tail) {
        return HAL_ERROR;
    }

    // Convert length to bytes
    uint32_t len = hal_dlc_code_byte_length(tx_msg_header->DataLength);

    // Don't overrun buffer element max length
    if (len > FDCAN_TX_QUEUE_DATA_LENGTH)
        return HAL_ERROR;

    tx_queue[itf].header[tx_queue[itf].head] = *tx_msg_header;

    for (uint8_t i = 0; i < len; i++) {
        tx_queue[itf].data[tx_queue[itf].head][i] = tx_msg_data[i];
    }

    // Increment the head pointer
    tx_queue[itf].head = (tx_queue[itf].head + 1) % FDCAN_TX_QUEUE_LENGTH;

    return HAL_OK;
}

/*!
 * \brief Handles the reception of a message from the FDCAN bus
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] rx_msg_header A pointer to an FDCAN_RxHeaderTypeDef structure describing the structure of the message
 * \param[in] rx_msg_data A pointer to the message data
 * \return The status of the operation (HAL_OK on success, HAL_ERROR on failure)
 */
uint32_t fdcan_receive(enum fdcan_itf itf, FDCAN_RxHeaderTypeDef *rx_msg_header, uint8_t *rx_msg_data) {
    uint32_t status;
    FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

    status = HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, rx_msg_header, rx_msg_data);

    led_yellow_on();
    return status;
}

/*!
 * \brief Handles the processing of the data from the FDCAN circular buffers
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 */
void fdcan_process(void) {
    for (uint8_t itf = 0; itf < FDCAN_ITF_COUNT; itf++) {
        FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

        while ((tx_queue[itf].tail != tx_queue[itf].head) && (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) > 0)) {
            HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &tx_queue[itf].header[tx_queue[itf].tail], tx_queue[itf].data[tx_queue[itf].tail]);
            tx_queue[itf].tail = (tx_queue[itf].tail + 1) % FDCAN_TX_QUEUE_LENGTH;

            led_green_on();
        }
    }
}

/*!
 * \brief Checks if a message has been received and is waiting in the queue
 *
 * \param[in] itf The enumeration value corresponding to one of the available FDCAN peripherals
 * \param[in] fifo An integer that corresponds to the location of the FIFO data structure
 * \return A value equivalent to true if a message is pending, one equivalent to false otherwise
 */
bool is_fdcan_msg_pending(enum fdcan_itf itf, uint8_t fifo) {
    FDCAN_HandleTypeDef *hfdcan = fdcan_get_handle_by_itf(itf);

    if (bus_state[itf] == OFF_BUS) {
        return false;
    }

    return (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, fifo) > 0);
}

/* USER CODE END 1 */
