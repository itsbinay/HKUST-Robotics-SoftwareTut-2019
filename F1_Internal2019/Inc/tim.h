/**
  ******************************************************************************
  * File Name          : TIM.h
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;

/* USER CODE BEGIN Private defines */
#define SERVO_TIM (&htim5)
#define MOTOR_TIM (&htim1)
#define ENCODER_1 (&htim4)
#define ENCODER_2 (&htim3)
#define ENCODER_3 (&htim8)
/* USER CODE END Private defines */

/**
 * @brief      Initialize TIM1 for PWM Output
 * 
 * This is the Timer connected to the motor ports
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_PWM_Start()
 * 
 */
void TIM1_PWM_Init(void);
/**
 * @brief      Initialize TIM5 for PWM Output
 * 
 * This is the Timer connected to the servo ports
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_PWM_Start()
 * 
 */
void TIM5_PWM_Init(void);

/**
 * @brief      Initialize TIM8 for Input Capture
 * 
 * This is the Timer connected to Encoder 3 port
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_IC_Start()
 * 
 */
void TIM8_IC_Init(void);
/**
 * @brief      Initialize TIM3 for Input Capture
 * 
 * This is the Timer connected to Encoder 2 port
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_IC_Start()
 * 
 */
void TIM3_IC_Init(void);
/**
 * @brief      Initialize TIM4 for Input Capture
 * 
 * This is the Timer connected to Encoder 1 port
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_IC_Start()
 * 
 */
void TIM4_IC_Init(void);
/**
 * @brief      Initialize TIM8 for Encoder Input
 * 
 * This is the Timer connected to Encoder 3 port
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_IC_Start()
 * 
 */
void TIM8_ENC_Init(void);
/**
 * @brief      Initialize TIM3 for Encoder Input
 * 
 * This is the Timer connected to Encoder 3 port
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_IC_Start()
 * 
 */
void TIM3_ENC_Init(void);
/**
 * @brief      Initialize TIM4 for Encoder Input
 * 
 * This is the Timer connected to Encoder 3 port
 * 
 * This function does not configure or start the Timer, see HAL_TIM_ConfigTimer() and HAL_TIM_IC_Start()
 * 
 */
void TIM4_ENC_Init(void);

/**
 * @brief      Initialize TIM6 for Internal Timing
 */
void TIM6_Init(void);
/**
 * @brief      Initialize TIM7 for Internal Timing
 * 
 * This is used by the Camera Library internally
 * 
 */
void TIM7_Init(void);
                        
/**
 * @brief      Internal Function for HAL Library
 */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                        
/* USER CODE BEGIN Prototypes */

/**
 * @brief      Configure the Prescaler and Autoreload of the a given Timer peripheral
 *
 * @param      htim        The Timer to configure
 * @param[in]  prescaler   The prescaler
 * @param[in]  autoreload  The autoreload value
 */
void HAL_TIM_ConfigTimer(TIM_HandleTypeDef* htim, uint16_t prescaler, uint16_t autoreload);

/**
 * @brief      Set the Output Compare value for a given Timer Channel
 *
 * @param      htim     The Timer
 * @param[in]  channel  The channel
 *  TIM_CHANNEL_1 TIM_CHANNEL_2 TIM_CHANNEL_3 TIM_CHANNEL_4
 * @param[in]  compare  The compare value
 */
void HAL_TIM_PWM_SetCompare(TIM_HandleTypeDef* htim, uint32_t channel, uint16_t compare);

/**
 * @brief      Get the Input Capture value for a given Timer Channel
 *
 * @param      htim     The Timer 
 * @param[in]  channel  The channel
 *  TIM_CHANNEL_1 TIM_CHANNEL_2 TIM_CHANNEL_3 TIM_CHANNEL_4
 *  
 * @return     Value in the Capture Register
 */
uint16_t HAL_TIM_PWM_GetCapture(TIM_HandleTypeDef* htim, uint32_t channel);

/**
 * @brief      Start/Stop PWM Output of a Timer Channel
 *
 * @param      htim     The Timer
 * @param[in]  channel  The channel
 *
 * @return     Success status of the operation
 */
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief      Start/Stop Encoder Input of a Timer Channel
 *
 * @param      htim     The Timer
 * @param[in]  Channel  The channel
 *
 * @return     Success status of the operation
 */
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

/**
 * @brief      Start/Stop Input Capture of a Timer Channel
 *
 * @param      htim     The Timer
 * @param[in]  Channel  The channel
 *
 * @return     Success status of the operation
 */
HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ tim_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
