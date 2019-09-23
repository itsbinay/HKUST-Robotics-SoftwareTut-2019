/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
 * @brief      Initialize USART1 with specified baudrate
 *
 * @param[in]  baudrate  The baudrate
 */
void USART1_Init(uint32_t baudrate);
/**
 * @brief      Initialize USART3 with specified baudrate
 *
 * @param[in]  baudrate  The baudrate
 */
void USART3_Init(uint32_t baudrate);

// USER CODE BEGIN Prototypes

/**
 * See official HAL Documentations
 */
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
/**
 * See official HAL Documentations
 */
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
/**
 * See official HAL Documentations
 */
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/**
 * @brief      Receive unlimited data and get a interrupt callback on each byte
 *
 * @param      huart            The UART
 * @param[in]  RxBytesCallback  The function to call with each byte
 *
 * @return     Success Status of the operation
 */
HAL_StatusTypeDef HAL_UART_ReceiveBytes_IT(UART_HandleTypeDef *huart, void (* RxBytesCallback)(struct __UART_HandleTypeDef *huart, const uint16_t byte));
/**
 * See official HAL Documentations
 */
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
/**
 * See official HAL Documentations
 */
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
