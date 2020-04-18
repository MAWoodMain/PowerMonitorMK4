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
#include <stdint.h>
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
#define SERIAL_INTERFACE_UNSOLICITED_CARD_CHANGE_TAG (uint8_t*)"CC"
#define SERIAL_INTERFACE_UNSOLICITED_SYMBOL '%'
#define REPLY_BUFFER_SIZE 200U
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
bool serialInterface_extractStrArg(uint8_t* argsString, uint8_t argIdx, uint8_t** value);
bool serialInterface_extractUintArg(uint8_t* argsString, uint8_t argIdx, uint32_t* value, uint8_t radix);
bool serialInterface_extractIntArg(uint8_t* argsString, uint8_t argIdx, int32_t* value, uint8_t radix);
bool serialInterface_extractLongDoubleArg(uint8_t* argsString, uint8_t argIdx, long double* value);
void serialInterface_Unsolicitedf(uint8_t* tag, const char *format, ...);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //SERIALINTERFACE_H