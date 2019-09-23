/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUZZER_Pin                        GPIO_PIN_3
#define BUZZER_GPIO_Port                  GPIOE

#define LED1_Pin                          GPIO_PIN_4
#define LED1_GPIO_Port                    GPIOE
#define LED2_Pin                          GPIO_PIN_5
#define LED2_GPIO_Port                    GPIOE
#define LED3_Pin                          GPIO_PIN_6
#define LED3_GPIO_Port                    GPIOE

#define SERVO1_Pin                        GPIO_PIN_0
#define SERVO1_GPIO_Port                  GPIOA
#define SERVO2_Pin                        GPIO_PIN_1
#define SERVO2_GPIO_Port                  GPIOA
#define SERVO3_Pin                        GPIO_PIN_2
#define SERVO3_GPIO_Port                  GPIOA

#define TFT_DC_Pin                        GPIO_PIN_4
#define TFT_DC_GPIO_Port                  GPIOA
#define TFT_SCL_Pin                       GPIO_PIN_5
#define TFT_SCL_GPIO_Port                 GPIOA
#define TFT_RES_Pin                       GPIO_PIN_6
#define TFT_RES_GPIO_Port                 GPIOA
#define TFT_SDA_Pin                       GPIO_PIN_7
#define TFT_SDA_GPIO_Port                 GPIOA

#define M1_PWM_Pin                        GPIO_PIN_9
#define M1_PWM_GPIO_Port                  GPIOE
#define M1_DIR_Pin                        GPIO_PIN_10
#define M1_DIR_GPIO_Port                  GPIOE
#define M2_PWM_Pin                        GPIO_PIN_11
#define M2_PWM_GPIO_Port                  GPIOE
#define M2_DIR_Pin                        GPIO_PIN_12
#define M2_DIR_GPIO_Port                  GPIOE
#define M3_PWM_Pin                        GPIO_PIN_13
#define M3_PWM_GPIO_Port                  GPIOE
#define M3_DIR_Pin                        GPIO_PIN_15
#define M3_DIR_GPIO_Port                  GPIOE
#define M4_PWM_Pin                        GPIO_PIN_14
#define M4_PWM_GPIO_Port                  GPIOE
#define M4_DIR_Pin                        GPIO_PIN_10
#define M4_DIR_GPIO_Port                  GPIOB

#define IO1_Pin                           GPIO_PIN_10
#define IO1_GPIO_Port                     GPIOD
#define IO2_Pin                           GPIO_PIN_11
#define IO2_GPIO_Port                     GPIOD
#define IO3_Pin                           GPIO_PIN_12
#define IO3_GPIO_Port                     GPIOD
#define IO4_Pin                           GPIO_PIN_13
#define IO4_GPIO_Port                     GPIOD
#define IO5_Pin                           GPIO_PIN_14
#define IO5_GPIO_Port                     GPIOD
#define IO6_Pin                           GPIO_PIN_15
#define IO6_GPIO_Port                     GPIOD
#define IO7_Pin                           GPIO_PIN_8
#define IO7_GPIO_Port                     GPIOC
#define IO8_Pin                           GPIO_PIN_9
#define IO8_GPIO_Port                     GPIOC
#define IO9_Pin                           GPIO_PIN_8
#define IO9_GPIO_Port                     GPIOA

#define BUTTON3_Pin                       GPIO_PIN_11
#define BUTTON3_GPIO_Port                 GPIOA
#define BUTTON2_Pin                       GPIO_PIN_12
#define BUTTON2_GPIO_Port                 GPIOA
#define BUTTON1_Pin                       GPIO_PIN_3
#define BUTTON1_GPIO_Port                 GPIOB

#define C_OE_Pin                          GPIO_PIN_12
#define C_OE_GPIO_Port                    GPIOC
#define C_RRST_Pin                        GPIO_PIN_11
#define C_RRST_GPIO_Port                  GPIOC
#define C_PCLK_Pin                        GPIO_PIN_10
#define C_PCLK_GPIO_Port                  GPIOC
#define C_VSYNC_Pin                       GPIO_PIN_15
#define C_VSYNC_GPIO_Port                 GPIOA
#define C_D0_Pin                          GPIO_PIN_0
#define C_D0_GPIO_Port                    GPIOD
#define C_D1_Pin                          GPIO_PIN_1
#define C_D1_GPIO_Port                    GPIOD
#define C_D2_Pin                          GPIO_PIN_2
#define C_D2_GPIO_Port                    GPIOD
#define C_D3_Pin                          GPIO_PIN_3
#define C_D3_GPIO_Port                    GPIOD
#define C_D4_Pin                          GPIO_PIN_4
#define C_D4_GPIO_Port                    GPIOD
#define C_D5_Pin                          GPIO_PIN_5
#define C_D5_GPIO_Port                    GPIOD
#define C_D6_Pin                          GPIO_PIN_6
#define C_D6_GPIO_Port                    GPIOD
#define C_D7_Pin                          GPIO_PIN_7
#define C_D7_GPIO_Port                    GPIOD
#define C_WRST_Pin                        GPIO_PIN_0
#define C_WRST_GPIO_Port                  GPIOE
#define C_WEN_Pin                         GPIO_PIN_1
#define C_WEN_GPIO_Port                   GPIOE
#define SCCB_SCL_Pin                      GPIO_PIN_8
#define SCCB_SCL_GPIO_Port                GPIOB
#define SCCB_SDA_Pin                      GPIO_PIN_9
#define SCCB_SDA_GPIO_Port                GPIOB

#define PWMI3_Pin                         GPIO_PIN_6
#define PWMI3_GPIO_Port                   GPIOC
#define PWMI2_Pin                         GPIO_PIN_4
#define PWMI2_GPIO_Port                   GPIOB
#define PWMI1_Pin                         GPIO_PIN_6
#define PWMI1_GPIO_Port                   GPIOB

#define ENC3A_Pin                         GPIO_PIN_7
#define ENC3A_GPIO_Port                   GPIOC
#define ENC3B_Pin                         GPIO_PIN_6
#define ENC3B_GPIO_Port                   GPIOC
#define ENC2A_Pin                         GPIO_PIN_4
#define ENC2A_GPIO_Port                   GPIOB
#define ENC2B_Pin                         GPIO_PIN_5
#define ENC2B_GPIO_Port                   GPIOB
#define ENC1A_Pin                         GPIO_PIN_6
#define ENC1A_GPIO_Port                   GPIOB
#define ENC1B_Pin                         GPIO_PIN_7
#define ENC1B_GPIO_Port                   GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
