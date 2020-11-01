/**
  * @file    slot1.c 
  * @brief    
  * @version 1.00   
  * @date    18/04/2020
  */

/**************************** LIB INCLUDES ******************************/
#include <limits.h>
/**************************** USER INCLUDES *****************************/
#include "cmsis_os2.h"
#include "slot4.h"
#include "card.h"
#include "slotConfig.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
/*************************** PRIVATE FUNCTIONS **************************/


_Noreturn void slot4_task(void* argument)
{
    static const uint8_t cardId = SLOT_4;

    uint32_t nextOp = osKernelGetTickCount() + SLOT_PROC_PERIOD;
    nextOp += ( SLOT_PROC_PERIOD * cardId ) / SLOT_PROC_PERIOD;
    card_init( cardId );
    card_ledTest( cardId );
    /* Infinite loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for (;;)
    {
        osDelayUntil( nextOp );
        nextOp += SLOT_PROC_PERIOD;
        card_process( cardId );
    }
#pragma clang diagnostic pop
}