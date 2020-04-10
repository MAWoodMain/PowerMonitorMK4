/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

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
#define SSA4_Pin GPIO_PIN_13
#define SSA4_GPIO_Port GPIOC
#define SSB4_Pin GPIO_PIN_14
#define SSB4_GPIO_Port GPIOC
#define SSC4_Pin GPIO_PIN_15
#define SSC4_GPIO_Port GPIOC
#define Board_Temp_Pin GPIO_PIN_1
#define Board_Temp_GPIO_Port GPIOA
#define SSA1_Pin GPIO_PIN_4
#define SSA1_GPIO_Port GPIOC
#define SSB1_Pin GPIO_PIN_5
#define SSB1_GPIO_Port GPIOC
#define SSC1_Pin GPIO_PIN_0
#define SSC1_GPIO_Port GPIOB
#define SSC2_Pin GPIO_PIN_1
#define SSC2_GPIO_Port GPIOB
#define SSB2_Pin GPIO_PIN_2
#define SSB2_GPIO_Port GPIOB
#define SSA2_Pin GPIO_PIN_7
#define SSA2_GPIO_Port GPIOE
#define SSA3_Pin GPIO_PIN_13
#define SSA3_GPIO_Port GPIOB
#define SSB3_Pin GPIO_PIN_14
#define SSB3_GPIO_Port GPIOB
#define SSC3_Pin GPIO_PIN_15
#define SSC3_GPIO_Port GPIOB
#define G1_Pin GPIO_PIN_8
#define G1_GPIO_Port GPIOD
#define R1_Pin GPIO_PIN_9
#define R1_GPIO_Port GPIOD
#define B1_Pin GPIO_PIN_10
#define B1_GPIO_Port GPIOD
#define G2_Pin GPIO_PIN_11
#define G2_GPIO_Port GPIOD
#define R2_Pin GPIO_PIN_12
#define R2_GPIO_Port GPIOD
#define B2_Pin GPIO_PIN_13
#define B2_GPIO_Port GPIOD
#define G3_Pin GPIO_PIN_14
#define G3_GPIO_Port GPIOD
#define R3_Pin GPIO_PIN_15
#define R3_GPIO_Port GPIOD
#define B3_Pin GPIO_PIN_6
#define B3_GPIO_Port GPIOC
#define G4_Pin GPIO_PIN_7
#define G4_GPIO_Port GPIOC
#define R4_Pin GPIO_PIN_8
#define R4_GPIO_Port GPIOC
#define B4_Pin GPIO_PIN_9
#define B4_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
