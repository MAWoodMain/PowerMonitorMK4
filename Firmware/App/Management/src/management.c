#include <limits.h>
/**
  * @file    management.c 
  * @brief    
  * @version 1.00   
  * @date    18/04/2020
  */

/**************************** LIB INCLUDES ******************************/
#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
/**************************** USER INCLUDES *****************************/
#include "management.h"
#include "cmsis_os2.h"
#include "debug.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
uint8_t freertos_getIdlePercentage(void);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
extern void *pxCurrentTCB;
static volatile uint32_t tickCount = 0;
static volatile uint32_t idleCount = 0;
/*************************** PUBLIC FUNCTIONS ***************************/
/*************************** PRIVATE FUNCTIONS **************************/

_Noreturn void management_task(void *argument)
{
    /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for(;;)
    {
        osDelay(1000);
        debug_sendf(LEVEL_DEBUG, "Idle: %d", freertos_getIdlePercentage());
    }
#pragma clang diagnostic pop
}

void vApplicationTickHook( void )
{
    uint8_t* taskName = (uint8_t*)pcTaskGetName( xTaskGetCurrentTaskHandle() );
    tickCount++;
    if(taskName[0] == 'I')
    {
        idleCount++;
    }
}
uint8_t freertos_getIdlePercentage(void)
{
    uint8_t retVal = 0U;
    taskENTER_CRITICAL();
    retVal = (idleCount*100) / (tickCount);
    idleCount = 0U;
    tickCount = 0U;
    taskEXIT_CRITICAL();
    return retVal;
}