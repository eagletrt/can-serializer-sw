/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usbd_cdc_if.c
 * @version        : v1.0_Cube
 * @brief          : Usb device for Virtual Com Port.
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
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */

#include "slcan.h"

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

struct UsbRxBuf rx_buf = { 0 };
struct UsbTxBuf tx_buf = { 0 };
uint8_t tx_linbuf[TX_LINBUF_SIZE] = { 0 };
uint8_t slcan_string[SLCAN_MAXIMUM_TRANSMISSION_UNIT];
uint8_t slcan_string_idx = 0;

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */
/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */

/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferHS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferHS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceHS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_HS(void);
static int8_t CDC_DeInit_HS(void);
static int8_t CDC_Control_HS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_HS(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_HS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_HS = {
    CDC_Init_HS,
    CDC_DeInit_HS,
    CDC_Control_HS,
    CDC_Receive_HS,
    CDC_TransmitCplt_HS
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media low layer over the USB HS IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_HS(void) {
    /* USER CODE BEGIN 8 */
    /* Set Application Buffers */
    rx_buf.head = 0;
    rx_buf.tail = 0;
    tx_buf.head = 0;
    tx_buf.tail = 0;

    USBD_CDC_SetTxBuffer(&hUsbDeviceHS, tx_linbuf, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceHS, rx_buf.buf[rx_buf.head]);
    return (USBD_OK);
    /* USER CODE END 8 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @param  None
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_HS(void) {
    /* USER CODE BEGIN 9 */
    return (USBD_OK);
    /* USER CODE END 9 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_HS(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
    /* USER CODE BEGIN 10 */
    switch (cmd) {
        case CDC_SEND_ENCAPSULATED_COMMAND:

            break;

        case CDC_GET_ENCAPSULATED_RESPONSE:

            break;

        case CDC_SET_COMM_FEATURE:

            break;

        case CDC_GET_COMM_FEATURE:

            break;

        case CDC_CLEAR_COMM_FEATURE:

            break;

        /*******************************************************************************/
        /* Line Coding Structure */
        /*-----------------------------------------------------------------------------*/
        /* Offset | Field       | Size | Value  | Description */
        /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per
   * second*/
        /* 4      | bCharFormat |   1  | Number | Stop bits */
        /*                                        0 - 1 Stop bit */
        /*                                        1 - 1.5 Stop bits */
        /*                                        2 - 2 Stop bits */
        /* 5      | bParityType |  1   | Number | Parity */
        /*                                        0 - None */
        /*                                        1 - Odd */
        /*                                        2 - Even */
        /*                                        3 - Mark */
        /*                                        4 - Space */
        /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16). */
        /*******************************************************************************/
        case CDC_SET_LINE_CODING:

            break;

        case CDC_GET_LINE_CODING:

            break;

        case CDC_SET_CONTROL_LINE_STATE:

            break;

        case CDC_SEND_BREAK:

            break;

        default:
            break;
    }

    return (USBD_OK);
    /* USER CODE END 10 */
}

/**
  * @brief Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAILL
  */
static int8_t CDC_Receive_HS(uint8_t *Buf, uint32_t *Len) {
    /* USER CODE BEGIN 11 */
    if (((rx_buf.head + 1) % RX_BUFS_NUM) == rx_buf.tail) {
        USBD_CDC_SetRxBuffer(&hUsbDeviceHS, &Buf[0]);
        USBD_CDC_ReceivePacket(&hUsbDeviceHS);
        return HAL_ERROR;
    } else {
        rx_buf.msg_len[rx_buf.head] = *Len;
        rx_buf.head = (rx_buf.head + 1) % RX_BUFS_NUM;

        USBD_CDC_SetRxBuffer(&hUsbDeviceHS, rx_buf.buf[rx_buf.head]);
        USBD_CDC_ReceivePacket(&hUsbDeviceHS);
        return (USBD_OK);
    }
    /* USER CODE END 11 */
}

/**
  * @brief  Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_HS(uint8_t *Buf, uint16_t Len) {
    uint8_t result = USBD_OK;
    /* USER CODE BEGIN 12 */
    USBD_CDC_HandleTypeDef *hcdc =
        (USBD_CDC_HandleTypeDef *)hUsbDeviceHS.pClassData;
    if (hcdc->TxState != 0) {
        return USBD_BUSY;
    }
    USBD_CDC_SetTxBuffer(&hUsbDeviceHS, Buf, Len);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceHS);
    /* USER CODE END 12 */
    return result;
}

/**
  * @brief  CDC_TransmitCplt_HS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_HS(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
    uint8_t result = USBD_OK;
    /* USER CODE BEGIN 14 */
    UNUSED(Buf);
    UNUSED(Len);
    UNUSED(epnum);

    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)hUsbDeviceHS.pClassData;
    if (hcdc != NULL) {
        hcdc->TxState = 0;
    }
    /* USER CODE END 14 */
    return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/*!
 * \brief Enqueues the data for transmission over USB CDC to the host
 *
 * \param[in] buf The buffer containing the data to transmit
 * \param[in] len The length of the buffer
 */
void cdc_transmit(uint8_t *buf, uint16_t len) {
    __disable_irq();
    __DSB();
    __ISB();
    if (((tx_buf.head + len) % APP_TX_DATA_SIZE) == tx_buf.tail) {
        return;
    } else {
        for (uint32_t i = 0; i < len; i++) {
            tx_buf.data[tx_buf.head] = buf[i];
            tx_buf.head = (tx_buf.head + 1UL) % APP_TX_DATA_SIZE;
        }
    }
    __enable_irq();
}

/*!
 * \brief Processes incoming and outgoing USB CDC data
 */
void cdc_process(void) {
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)hUsbDeviceHS.pClassData;
    if (hcdc->TxState == 0) {
        uint16_t linbuf_ctr = 0;
        while (tx_buf.tail != tx_buf.head) {
            tx_linbuf[linbuf_ctr++] = tx_buf.data[tx_buf.tail];
            tx_buf.tail = (tx_buf.tail + 1UL) % APP_TX_DATA_SIZE;

            // Take up to the number of bytes to fill the linbuf
            if (linbuf_ctr >= TX_LINBUF_SIZE)
                break;
        }

        if (linbuf_ctr > 0) {
            USBD_CDC_SetTxBuffer(&hUsbDeviceHS, tx_linbuf, linbuf_ctr);
            USBD_CDC_TransmitPacket(&hUsbDeviceHS);
        }
    }

    __disable_irq();
    __DSB();
    __ISB();
    if (rx_buf.tail != rx_buf.head) {
        for (uint32_t i = 0; i < rx_buf.msg_len[rx_buf.tail]; i++) {
            if (rx_buf.buf[rx_buf.tail][i] == '\r') {
                slcan_parse_string(slcan_string, slcan_string_idx);
                slcan_string_idx = 0;
            } else {
                if (slcan_string_idx >= SLCAN_MAXIMUM_TRANSMISSION_UNIT) {
                    slcan_string_idx = 0;
                }

                slcan_string[slcan_string_idx++] = rx_buf.buf[rx_buf.tail][i];
            }
        }

        rx_buf.tail = (rx_buf.tail + 1) % RX_BUFS_NUM;
    }
    __enable_irq();
}

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
