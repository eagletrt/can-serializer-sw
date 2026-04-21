/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "eagletrt-api.h"
#include <stdint.h>

/* USER CODE BEGIN 0 */

EAGLETRT_STATIC EAGLETRT_VOLATILE uint32_t led_yellow_last_on = 0;
EAGLETRT_STATIC EAGLETRT_VOLATILE uint32_t led_green_last_on = 0;
EAGLETRT_STATIC EAGLETRT_VOLATILE uint32_t led_red_last_on = 0;
EAGLETRT_STATIC uint32_t led_yellow_last_off = 0;
EAGLETRT_STATIC uint32_t led_green_last_off = 0;
EAGLETRT_STATIC uint32_t led_red_last_off = 0;
EAGLETRT_STATIC uint8_t error_blink_status = 0;
EAGLETRT_STATIC uint8_t error_was_indicating = 0;
EAGLETRT_STATIC uint32_t last_errflash = 0;

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PD8   ------> USART3_TX
     PD9   ------> USART3_RX
*/
void MX_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : STLK_VCP_RX_Pin STLK_VCP_TX_Pin */
    GPIO_InitStruct.Pin = STLK_VCP_RX_Pin | STLK_VCP_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

void led_init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /*Configure GPIO pins : LED_GREEN_Pin LED_RED_Pin */
    GPIO_InitStruct.Pin = LED_GREEN_Pin | LED_RED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : LED_YELLOW_Pin */
    GPIO_InitStruct.Pin = LED_YELLOW_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_YELLOW_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LED_GREEN, 1U);
}

/*!
 * \brief Turns on the green LED
 */
void led_green_on(void) {
    if (led_green_last_on == 0U && (HAL_GetTick() - led_green_last_off) > LED_DURATION) {
        HAL_GPIO_WritePin(LED_GREEN, 0U);
        led_green_last_on = HAL_GetTick();
    }
}

/*!
 * \brief Turns off the green LED
 */
void led_green_off(void) {
    HAL_GPIO_WritePin(LED_GREEN, 1U);
}

/*!
 * \brief Makes the yellow LED blink
 *
 * \param[in] num_blinks The desidered number of blinks
 */
void led_yellow_blink(uint8_t num_blinks) {
    for (uint8_t i = 0; i < num_blinks; i++) {
        HAL_GPIO_WritePin(LED_YELLOW, 1U);
        HAL_Delay(100);
        HAL_GPIO_WritePin(LED_YELLOW, 0U);
        HAL_Delay(100);
    }
}

/*!
 * \brief Turns on the yellow LED
 */
void led_yellow_on(void) {
    if (led_yellow_last_on == 0 && (HAL_GetTick() - led_yellow_last_off) > LED_DURATION) {
        HAL_GPIO_WritePin(LED_YELLOW, 1U);
        led_yellow_last_on = HAL_GetTick();
    }
}

/*!
 * \brief Turns off the yellow LED
 */
void led_yellow_off(void) {
    HAL_GPIO_WritePin(LED_YELLOW, 1U);
}

/*!
 * \brief Turns on the red LED
 */
void led_red_on(void) {
    if (led_red_last_on == 0 && (HAL_GetTick() - led_red_last_off) > LED_DURATION) {
        HAL_GPIO_WritePin(LED_RED, 0U);
        led_red_last_on = HAL_GetTick();
    }
}

/*!
 * \brief Turns off the red LED
 */
void led_red_off(void) {
    HAL_GPIO_WritePin(LED_RED, 1U);
}

/*!
 * \brief Processes time-based LED events
 */
void led_process(void) {
    // If we were blinking but no longer are blinking, turn the power LED back on.
    if (error_was_indicating) {
        HAL_GPIO_WritePin(LED_GREEN, 1U);
        error_was_indicating = 0;
    }

    // If LED has been on for long enough, turn it off
    if (led_yellow_last_on > 0 && (HAL_GetTick() - led_yellow_last_on) > LED_DURATION) {
        HAL_GPIO_WritePin(LED_YELLOW, 0U);
        led_yellow_last_on = 0;
        led_yellow_last_off = HAL_GetTick();
    }

    // If LED has been on for long enough, turn it off
    if (led_green_last_on > 0 && (HAL_GetTick() - led_green_last_on) > LED_DURATION) {
        // Invert LED
        HAL_GPIO_WritePin(LED_GREEN, 1U);
        led_green_last_on = 0;
        led_green_last_off = HAL_GetTick();
    }

    // If LED has been on for long enough, turn it off
    if (led_red_last_on > 0 && (HAL_GetTick() - led_red_last_on) > LED_DURATION) {
        HAL_GPIO_WritePin(LED_RED, 1U);
        led_red_last_on = 0;
        led_red_last_off = HAL_GetTick();
    }
}

/* USER CODE END 2 */
