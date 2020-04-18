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
#include "printf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
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
/* USER CODE END Variables */
/* Definitions for MNMT_task */
osThreadId_t MNMT_taskHandle;
uint32_t MNMT_taskBuffer[ 4096 ];
osStaticThreadDef_t MNMT_taskControlBlock;
const osThreadAttr_t MNMT_task_attributes = {
  .name = "MNMT_task",
  .stack_mem = &MNMT_taskBuffer[0],
  .stack_size = sizeof(MNMT_taskBuffer),
  .cb_mem = &MNMT_taskControlBlock,
  .cb_size = sizeof(MNMT_taskControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SI_task */
osThreadId_t SI_taskHandle;
uint32_t SI_taskBuffer[ 512 ];
osStaticThreadDef_t SI_taskControlBlock;
const osThreadAttr_t SI_task_attributes = {
  .name = "SI_task",
  .stack_mem = &SI_taskBuffer[0],
  .stack_size = sizeof(SI_taskBuffer),
  .cb_mem = &SI_taskControlBlock,
  .cb_size = sizeof(SI_taskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for S1_task */
osThreadId_t S1_taskHandle;
uint32_t S1_taskBuffer[ 2048 ];
osStaticThreadDef_t S1_taskControlBlock;
const osThreadAttr_t S1_task_attributes = {
  .name = "S1_task",
  .stack_mem = &S1_taskBuffer[0],
  .stack_size = sizeof(S1_taskBuffer),
  .cb_mem = &S1_taskControlBlock,
  .cb_size = sizeof(S1_taskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for S2_task */
osThreadId_t S2_taskHandle;
uint32_t S2_taskBuffer[ 2048 ];
osStaticThreadDef_t S2_taskControlBlock;
const osThreadAttr_t S2_task_attributes = {
  .name = "S2_task",
  .stack_mem = &S2_taskBuffer[0],
  .stack_size = sizeof(S2_taskBuffer),
  .cb_mem = &S2_taskControlBlock,
  .cb_size = sizeof(S2_taskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for S3_task */
osThreadId_t S3_taskHandle;
uint32_t S3_taskBuffer[ 2048 ];
osStaticThreadDef_t S3_taskControlBlock;
const osThreadAttr_t S3_task_attributes = {
  .name = "S3_task",
  .stack_mem = &S3_taskBuffer[0],
  .stack_size = sizeof(S3_taskBuffer),
  .cb_mem = &S3_taskControlBlock,
  .cb_size = sizeof(S3_taskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for S4_task */
osThreadId_t S4_taskHandle;
uint32_t S4_taskBuffer[ 2048 ];
osStaticThreadDef_t S4_taskControlBlock;
const osThreadAttr_t S4_task_attributes = {
  .name = "S4_task",
  .stack_mem = &S4_taskBuffer[0],
  .stack_size = sizeof(S4_taskBuffer),
  .cb_mem = &S4_taskControlBlock,
  .cb_size = sizeof(S4_taskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

extern void management_task(void *argument);
extern void serialInterface_task(void *argument);
extern void slot1_task(void *argument);
extern void slot2_task(void *argument);
extern void slot3_task(void *argument);
extern void slot4_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

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
  /* creation of MNMT_task */
  MNMT_taskHandle = osThreadNew(management_task, NULL, &MNMT_task_attributes);

  /* creation of SI_task */
  SI_taskHandle = osThreadNew(serialInterface_task, NULL, &SI_task_attributes);

  /* creation of S1_task */
  S1_taskHandle = osThreadNew(slot1_task, NULL, &S1_task_attributes);

  /* creation of S2_task */
  S2_taskHandle = osThreadNew(slot2_task, NULL, &S2_task_attributes);

  /* creation of S3_task */
  S3_taskHandle = osThreadNew(slot3_task, NULL, &S3_task_attributes);

  /* creation of S4_task */
  S4_taskHandle = osThreadNew(slot4_task, NULL, &S4_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_management_task */
/**
  * @brief  Function implementing the MNMT_task thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_management_task */
__weak void management_task(void *argument)
{
  /* USER CODE BEGIN management_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END management_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
