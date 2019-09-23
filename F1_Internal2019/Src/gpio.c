/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "stm32f1xx_it.h"
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
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, BUZZER_Pin|LED1_Pin|LED2_Pin|LED3_Pin 
                          |M1_DIR_Pin|M2_DIR_Pin|M3_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TFT_DC_Pin|TFT_RES_Pin, GPIO_PIN_RESET);
  
  /*Configure GPIO pin Output Level */
  //TODO: Best defaults for camera pins??
  HAL_GPIO_WritePin(C_OE_GPIO_Port, C_OE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(C_WEN_GPIO_Port, C_WEN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(C_WRST_GPIO_Port, C_WRST_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(C_RRST_GPIO_Port, C_RRST_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(C_PCLK_GPIO_Port, C_PCLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(M4_DIR_GPIO_Port, M4_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, IO1_Pin|IO2_Pin|IO3_Pin|IO4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin 
                           PEPin PEPin PEPin PEPin 
                           PEPin */
  GPIO_InitStruct.Pin = BUZZER_Pin|M1_DIR_Pin|M2_DIR_Pin|M3_DIR_Pin|C_WRST_Pin|C_WEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = TFT_DC_Pin|TFT_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = M4_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(M4_DIR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = C_VSYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(C_VSYNC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin PDPin 
                           PDPin PDPin */
  GPIO_InitStruct.Pin = C_D0_Pin|C_D1_Pin 
                          |C_D2_Pin|C_D3_Pin|C_D4_Pin|C_D5_Pin 
                          |C_D6_Pin|C_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = C_RRST_Pin|C_PCLK_Pin|C_OE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, NVIC_Priority_EXTI0_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_SetPriority(EXTI1_IRQn, NVIC_Priority_EXTI1_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  HAL_NVIC_SetPriority(EXTI2_IRQn, NVIC_Priority_EXTI2_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  HAL_NVIC_SetPriority(EXTI3_IRQn, NVIC_Priority_EXTI3_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  HAL_NVIC_SetPriority(EXTI4_IRQn, NVIC_Priority_EXTI4_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, NVIC_Priority_EXTI9_5_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, NVIC_Priority_EXTI15_10_IRQn, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 2 */
void HAL_GPIO_InitPin(GPIO_TypeDef* port, uint16_t pin, uint16_t mode, uint16_t pull) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = mode;
  GPIO_InitStruct.Pull = pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}

__weak void ADC_1_Interrupt(void); //0
__weak void ADC_2_Interrupt(void); //1
__weak void ADC_3_Interrupt(void); //2
__weak void ADC_4_Interrupt(void); //3
__weak void ENC2A_Interrupt(void); //4
__weak void ENC2B_Interrupt(void); //5
__weak void ENC1A_Interrupt(void); //6
__weak void ENC1B_Interrupt(void); //7
__weak void GPIO7_Interrupt(void); //8
__weak void GPIO8_Interrupt(void); //9
__weak void GPIO1_Interrupt(void); //10
__weak void GPIO2_Interrupt(void); //11
__weak void GPIO3_Interrupt(void); //12
__weak void GPIO4_Interrupt(void); //13
__weak void GPIO5_Interrupt(void); //14
__weak void VSYNC_Interrupt(void); //15

void HAL_GPIO_EXTI_Callback(uint16_t pin) {
#if 0
  switch (16 - __CLZ(pin)) {
    case 0:  ADC_1_Interrupt(); break;
    case 1:  ADC_2_Interrupt(); break;
    case 2:  ADC_3_Interrupt(); break;
    case 3:  ADC_4_Interrupt(); break;
    case 4:  ENC1A_Interrupt(); break;
    case 5:  ENC1B_Interrupt(); break;
    case 6:  ENC2A_Interrupt(); break;
    case 7:  ENC2B_Interrupt(); break;
    case 8:  GPIO7_Interrupt(); break;
    case 9:  GPIO8_Interrupt(); break;
    case 10: GPIO1_Interrupt(); break;
    case 11: GPIO2_Interrupt(); break;
    case 12: VSYNC_Interrupt(); break;
    case 13: GPIO4_Interrupt(); break;
    case 14: GPIO5_Interrupt(); break;
    case 15: GPIO6_Interrupt(); break;
    default: break;
  }
#else
  switch (pin) {
    case (1<<0):  ADC_1_Interrupt(); break;
    case (1<<1):  ADC_2_Interrupt(); break;
    case (1<<2):  ADC_3_Interrupt(); break;
    case (1<<3):  ADC_4_Interrupt(); break;
    case (1<<4):  ENC1A_Interrupt(); break;
    case (1<<5):  ENC1B_Interrupt(); break;
    case (1<<6):  ENC2A_Interrupt(); break;
    case (1<<7):  ENC2B_Interrupt(); break;
    case (1<<8):  GPIO7_Interrupt(); break;
    case (1<<9):  GPIO8_Interrupt(); break;
    case (1<<10): GPIO1_Interrupt(); break;
    case (1<<11): GPIO2_Interrupt(); break;
    case (1<<12): GPIO3_Interrupt(); break;
    case (1<<13): GPIO4_Interrupt(); break;
    case (1<<14): GPIO5_Interrupt(); break;
    case (1<<15): VSYNC_Interrupt(); break;
    default: break;
  }
#endif
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
