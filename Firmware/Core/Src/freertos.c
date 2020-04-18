#include <limits.h>
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "serialInterface.h"
#include "card.h"
#include "printf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
typedef enum
{
    CARD_0 = (uint8_t)0U,
    CARD_1 = (uint8_t)1U,
    CARD_2 = (uint8_t)2U,
    CARD_3 = (uint8_t)3U,
} cardId_e;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NO_CARDS 4U
#define CARD_PROC_PERIOD 1000U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
cardId_e cardIds[] = {CARD_0, CARD_1, CARD_2, CARD_3};
/* USER CODE END Variables */
/* Definitions for PROC_task */
osThreadId_t PROC_taskHandle;
uint32_t PROC_taskBuffer[ 4096 ];
osStaticThreadDef_t PROC_taskControlBlock;
const osThreadAttr_t PROC_task_attributes = {
  .name = "PROC_task",
  .stack_mem = &PROC_taskBuffer[0],
  .stack_size = sizeof(PROC_taskBuffer),
  .cb_mem = &PROC_taskControlBlock,
  .cb_size = sizeof(PROC_taskControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SI_task */
osThreadId_t SI_taskHandle;
const osThreadAttr_t SI_task_attributes = {
  .name = "SI_task",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};
/* Definitions for card0 */
osThreadId_t card0Handle;
uint32_t card0Buffer[ 2048 ];
osStaticThreadDef_t card0ControlBlock;
const osThreadAttr_t card0_attributes = {
  .name = "card0",
  .stack_mem = &card0Buffer[0],
  .stack_size = sizeof(card0Buffer),
  .cb_mem = &card0ControlBlock,
  .cb_size = sizeof(card0ControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for card1 */
osThreadId_t card1Handle;
uint32_t card1Buffer[ 2048 ];
osStaticThreadDef_t card1ControlBlock;
const osThreadAttr_t card1_attributes = {
  .name = "card1",
  .stack_mem = &card1Buffer[0],
  .stack_size = sizeof(card1Buffer),
  .cb_mem = &card1ControlBlock,
  .cb_size = sizeof(card1ControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for card2 */
osThreadId_t card2Handle;
uint32_t card2Buffer[ 2048 ];
osStaticThreadDef_t card2ControlBlock;
const osThreadAttr_t card2_attributes = {
  .name = "card2",
  .stack_mem = &card2Buffer[0],
  .stack_size = sizeof(card2Buffer),
  .cb_mem = &card2ControlBlock,
  .cb_size = sizeof(card2ControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for card3 */
osThreadId_t card3Handle;
uint32_t card3Buffer[ 2048 ];
osStaticThreadDef_t card3ControlBlock;
const osThreadAttr_t card3_attributes = {
  .name = "card3",
  .stack_mem = &card3Buffer[0],
  .stack_size = sizeof(card3Buffer),
  .cb_mem = &card3ControlBlock,
  .cb_size = sizeof(card3ControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
_Noreturn /* USER CODE END FunctionPrototypes */

void proc_task(void *argument);
extern void serialInterface_task(void *argument);
_Noreturn void card0_task(void *argument);
_Noreturn void card1_task(void *argument);
_Noreturn void card2_task(void *argument);
_Noreturn void card3_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of PROC_task */
  PROC_taskHandle = osThreadNew(proc_task, NULL, &PROC_task_attributes);

  /* creation of SI_task */
  SI_taskHandle = osThreadNew(serialInterface_task, NULL, &SI_task_attributes);

  /* creation of card0 */
  card0Handle = osThreadNew(card0_task, (void*) &cardIds[0], &card0_attributes);

  /* creation of card1 */
  card1Handle = osThreadNew(card1_task, (void*) &cardIds[1], &card1_attributes);

  /* creation of card2 */
  card2Handle = osThreadNew(card2_task, (void*) &cardIds[2], &card2_attributes);

  /* creation of card3 */
  card3Handle = osThreadNew(card3_task, (void*) &cardIds[3], &card3_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_proc_task */
/**
  * @brief  Function implementing the PROC_task thread.
  * @param  argument: Not used 
  * @retval None
  */
_Noreturn /* USER CODE END Header_proc_task */
void proc_task(void *argument)
{
  /* USER CODE BEGIN proc_task */
    uint8_t i;

    serialInterface_init();

    /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for(;;)
    {
        osDelay(1000);
        HAL_GPIO_WritePin(G4_GPIO_Port, G4_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(G4_GPIO_Port, G4_Pin, GPIO_PIN_SET);
    }
#pragma clang diagnostic pop
  /* USER CODE END proc_task */
}

/* USER CODE BEGIN Header_card0_task */
/**
* @brief Function implementing the card0 thread.
* @param argument: Not used
* @retval None
*/
_Noreturn /* USER CODE END Header_card0_task */
void card0_task(void *argument)
{
  /* USER CODE BEGIN card0_task */
  cardId_e cardId = *((cardId_e*)argument);

  uint32_t nextOp = osKernelGetTickCount() + CARD_PROC_PERIOD;
  nextOp += (CARD_PROC_PERIOD * (uint8_t) cardId)/CARD_PROC_PERIOD;
  card_init( (uint8_t) cardId );
  /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
  for(;;)
  {
      osDelayUntil(nextOp);
      nextOp+=CARD_PROC_PERIOD;
      card_process((uint8_t) cardId);
  }
#pragma clang diagnostic pop
  /* USER CODE END card0_task */
}

/* USER CODE BEGIN Header_card1_task */
/**
* @brief Function implementing the card1 thread.
* @param argument: Not used
* @retval None
*/
_Noreturn /* USER CODE END Header_card1_task */
void card1_task(void *argument)
{
  /* USER CODE BEGIN card1_task */
    cardId_e cardId = *((cardId_e*)argument);

    uint32_t nextOp = osKernelGetTickCount() + CARD_PROC_PERIOD;
    nextOp += (CARD_PROC_PERIOD * (uint8_t) cardId)/CARD_PROC_PERIOD;
    card_init( (uint8_t) cardId );
    /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for(;;)
    {
        osDelayUntil(nextOp);
        nextOp+=CARD_PROC_PERIOD;
        card_process((uint8_t) cardId);
    }
#pragma clang diagnostic pop
  /* USER CODE END card1_task */
}

/* USER CODE BEGIN Header_card2_task */
/**
* @brief Function implementing the card2 thread.
* @param argument: Not used
* @retval None
*/
_Noreturn /* USER CODE END Header_card2_task */
void card2_task(void *argument)
{
  /* USER CODE BEGIN card2_task */
    cardId_e cardId = *((cardId_e*)argument);

    uint32_t nextOp = osKernelGetTickCount() + CARD_PROC_PERIOD;
    nextOp += (CARD_PROC_PERIOD * (uint8_t) cardId)/CARD_PROC_PERIOD;
    card_init( (uint8_t) cardId );
    /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for(;;)
    {
        osDelayUntil(nextOp);
        nextOp+=CARD_PROC_PERIOD;
        card_process((uint8_t) cardId);
    }
#pragma clang diagnostic pop
  /* USER CODE END card2_task */
}

/* USER CODE BEGIN Header_card3_task */
/**
* @brief Function implementing the card3 thread.
* @param argument: Not used
* @retval None
*/
_Noreturn /* USER CODE END Header_card3_task */
void card3_task(void *argument)
{
  /* USER CODE BEGIN card3_task */
    cardId_e cardId = *((cardId_e*)argument);

    uint32_t nextOp = osKernelGetTickCount() + CARD_PROC_PERIOD;
    nextOp += (CARD_PROC_PERIOD * (uint8_t) cardId)/CARD_PROC_PERIOD;
    card_init( (uint8_t) cardId );
    /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for(;;)
    {
        osDelayUntil(nextOp);
        nextOp+=CARD_PROC_PERIOD;
        card_process((uint8_t) cardId);
    }
#pragma clang diagnostic pop
  /* USER CODE END card3_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
