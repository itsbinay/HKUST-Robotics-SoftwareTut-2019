/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

//  

/**
 * @brief      Initialize a Pin
 * 
 * For Reference:
 * 
 * GPIO Modes:
 *  - GPIO_MODE_INPUT
 *  - GPIO_MODE_OUTPUT_PP
 *  - GPIO_MODE_OUTPUT_OD
 *  - GPIO_MODE_IT_RISING
 *  - GPIO_MODE_IT_FALLING
 *  - GPIO_MODE_IT_RISING_FALLING
 *  - GPIO_MODE_ANALOG
 *  - GPIO_MODE_AF_PP
 *  - GPIO_MODE_AF_OD
 *  - GPIO_MODE_AF_INPUT
 *  
 * GPIO Pull:
 *  - GPIO_NOPULL
 *  - GPIO_PULLUP
 *  - GPIO_PULLDOWN 
 *
 * @param      port  The port
 * @param[in]  pin   The pin
 * @param[in]  mode  The mode
 * @param[in]  pull  The pull
 */
void HAL_GPIO_InitPin(GPIO_TypeDef* port, uint16_t pin, uint16_t mode, uint16_t pull);

/**
 * @brief      Read the input of a GPIO Pin
 *
 * @param      GPIOx     The gpio port
 * @param[in]  GPIO_Pin  The gpio pin
 *
 * @return     0 or 1
 */
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/**
 * @brief      Write a value to output of a GPIO Pin
 *
 * @param      GPIOx     The gpio port
 * @param[in]  GPIO_Pin  The gpio pin
 * @param[in]  PinState  0 or 1
 */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

/**
 * @brief      Toggle the value of output of a GPIO Pin
 *
 * @param      GPIOx     The gpio port
 * @param[in]  GPIO_Pin  The gpio pin
 */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

// The following pins support triggering an interrupt, 
// simply InitPin correctly and define a function like below to receive the interrupt
// void ADC_1_Interrupt(void) { /* Do Something */ }
// void ADC_2_Interrupt(void) { /* Do Something */ }
// void ADC_3_Interrupt(void) { /* Do Something */ }
// void ADC_4_Interrupt(void) { /* Do Something */ }
// void ENC1A_Interrupt(void) { /* Do Something */ }
// void ENC1B_Interrupt(void) { /* Do Something */ }
// void ENC2A_Interrupt(void) { /* Do Something */ }
// void ENC2B_Interrupt(void) { /* Do Something */ }
// void GPIO7_Interrupt(void) { /* Do Something */ }
// void GPIO8_Interrupt(void) { /* Do Something */ }
// void GPIO1_Interrupt(void) { /* Do Something */ }
// void GPIO2_Interrupt(void) { /* Do Something */ }
// void VSYNC_Interrupt(void) { /* Do Something */ } //Used by Camera Library
// void GPIO4_Interrupt(void) { /* Do Something */ }
// void GPIO5_Interrupt(void) { /* Do Something */ }
// void GPIO6_Interrupt(void) { /* Do Something */ }

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
