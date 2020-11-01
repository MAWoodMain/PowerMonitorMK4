/**
  * @file    slotConfig.h 
  * @brief    
  * @version 1.00   
  * @date    18/04/2020
  */

#ifndef SLOTCONFIG_H
#define SLOTCONFIG_H

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
#define SLOT_PROC_PERIOD pdMS_TO_TICKS(1000U)
/******************************** ENUMS *********************************/
typedef enum
{
    SLOT_1 = 0U,
    SLOT_2 = 1U,
    SLOT_3 = 2U,
    SLOT_4 = 3U
} SLOT_ID_E;
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //SLOTCONFIG_H