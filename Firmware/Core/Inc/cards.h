/**
  * @file    cards.h 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

#ifndef CARDS_H
#define CARDS_H

/**************************** LIB INCLUDES ******************************/
#include <stdbool.h>
#include <stdint.h>
/**************************** USER INCLUDES *****************************/
#include "serialInterface.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool cards_init(void);
void cards_commandReadRawHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARDS_H