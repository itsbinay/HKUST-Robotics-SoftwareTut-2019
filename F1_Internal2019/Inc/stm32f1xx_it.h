/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// Interrupt Priorities Centralized Here:
// Lower number (high priority) can interrupt higher number (low priority)
// Hence user code should be run at low priorities, while system services that user code relies on runs at high priorities
#define NVIC_Priority_USART3_IRQn          8 //USART3_RX
#define NVIC_Priority_USART1_IRQn          7 //USART1_RX
#define NVIC_Priority_DMA1_Channel1_IRQn   6 //DMA ADC1
#define NVIC_Priority_DMA1_Channel2_IRQn   5 //DMA USART3_TX
#define NVIC_Priority_DMA1_Channel4_IRQn   4 //DMA USART1_TX
#define NVIC_Priority_EXTI0_IRQn           3 //ADC_1
#define NVIC_Priority_EXTI1_IRQn           3 //ADC_2
#define NVIC_Priority_EXTI2_IRQn           3 //ADC_3
#define NVIC_Priority_EXTI3_IRQn           3 //ADC_4
#define NVIC_Priority_EXTI4_IRQn           3 //ENC1A
#define NVIC_Priority_EXTI9_5_IRQn         3 //ENC1B, ENC2A, ENC2B, GPIO7, GPIO8
#define NVIC_Priority_EXTI15_10_IRQn       3 //GPIO1, GPIO2, VSYNC, GPIO4, GPIO5, GPIO6
#define NVIC_Priority_DMA1_Channel3_IRQn   1 //DMA SPI1_TX
//SysTickPriority is 0 (Interrupt everything)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel1_IRQHandler(void);
void DMA1_Channel2_IRQHandler(void);
void DMA1_Channel3_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);
void USART1_IRQHandler(void);
void USART3_IRQHandler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
