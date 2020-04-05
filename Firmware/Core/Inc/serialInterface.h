/**
  * @file    serialInterface.h 
  * @brief    
  * @version 1.00   
  * @date    05/04/2020
  */

#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

/**************************** LIB INCLUDES ******************************/
#include <stdbool.h>
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
#define SERIAL_INTERFACE_UNSOLICITED_SYMBOL '%'
/******************************** ENUMS *********************************/
typedef enum
{
    OPERATION_SET = 0U,
    OPERATION_QUERY = 1U,
    OPERATION_RESET = 2U,
    /* !!! Update NO_OPERATIONS if any more added !!! */
    OPERATION_UNRECOGNISED = 0xFFU
} serialInterface_operation_e;
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool serialInterface_init(void);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //SERIALINTERFACE_H