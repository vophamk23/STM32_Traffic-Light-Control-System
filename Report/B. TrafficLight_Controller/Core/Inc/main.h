/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
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
extern "C"
{
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
#define RED1_Pin GPIO_PIN_3
#define RED1_GPIO_Port GPIOA
#define GREEN1_Pin GPIO_PIN_4
#define GREEN1_GPIO_Port GPIOA
#define YELLOW1_Pin GPIO_PIN_5
#define YELLOW1_GPIO_Port GPIOA
#define RED2_Pin GPIO_PIN_6
#define RED2_GPIO_Port GPIOA
#define GREEN2_Pin GPIO_PIN_7
#define GREEN2_GPIO_Port GPIOA
#define inputseg1_0_Pin GPIO_PIN_0
#define inputseg1_0_GPIO_Port GPIOB
#define inputseg1_1_Pin GPIO_PIN_1
#define inputseg1_1_GPIO_Port GPIOB
#define inputseg1_2_Pin GPIO_PIN_2
#define inputseg1_2_GPIO_Port GPIOB
#define inputseg3_2_Pin GPIO_PIN_10
#define inputseg3_2_GPIO_Port GPIOB
#define inputseg3_3_Pin GPIO_PIN_11
#define inputseg3_3_GPIO_Port GPIOB
#define inputmode_0_Pin GPIO_PIN_12
#define inputmode_0_GPIO_Port GPIOB
#define inputmode_1_Pin GPIO_PIN_13
#define inputmode_1_GPIO_Port GPIOB
#define inputmode_2_Pin GPIO_PIN_14
#define inputmode_2_GPIO_Port GPIOB
#define inputmode_3_Pin GPIO_PIN_15
#define inputmode_3_GPIO_Port GPIOB
#define YELLOW2_Pin GPIO_PIN_8
#define YELLOW2_GPIO_Port GPIOA
#define button1_Pin GPIO_PIN_9
#define button1_GPIO_Port GPIOA
#define button2_Pin GPIO_PIN_10
#define button2_GPIO_Port GPIOA
#define button3_Pin GPIO_PIN_11
#define button3_GPIO_Port GPIOA
#define inputseg0_0_Pin GPIO_PIN_12
#define inputseg0_0_GPIO_Port GPIOA
#define inputseg0_1_Pin GPIO_PIN_13
#define inputseg0_1_GPIO_Port GPIOA
#define inputseg0_2_Pin GPIO_PIN_14
#define inputseg0_2_GPIO_Port GPIOA
#define inputseg0_3_Pin GPIO_PIN_15
#define inputseg0_3_GPIO_Port GPIOA
#define inputseg1_3_Pin GPIO_PIN_3
#define inputseg1_3_GPIO_Port GPIOB
#define inputseg2_0_Pin GPIO_PIN_4
#define inputseg2_0_GPIO_Port GPIOB
#define inputseg2_1_Pin GPIO_PIN_5
#define inputseg2_1_GPIO_Port GPIOB
#define inputseg2_2_Pin GPIO_PIN_6
#define inputseg2_2_GPIO_Port GPIOB
#define inputseg2_3_Pin GPIO_PIN_7
#define inputseg2_3_GPIO_Port GPIOB
#define inputseg3_0_Pin GPIO_PIN_8
#define inputseg3_0_GPIO_Port GPIOB
#define inputseg3_1_Pin GPIO_PIN_9
#define inputseg3_1_GPIO_Port GPIOB
  /* USER CODE BEGIN Private defines */

  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
