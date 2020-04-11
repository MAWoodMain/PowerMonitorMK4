/**
  * @file    card.h 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#ifndef CARD_H
#define CARD_H

/**************************** LIB INCLUDES ******************************/
#include <stdbool.h>
/**************************** USER INCLUDES *****************************/
#include "cardIndicator.h"
#include "cardSpi.h"
#include "cardFilesystem.h"
#include "cardTypes.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool card_init(uint8_t cardId);
bool card_isPresent(uint8_t cardId);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARD_H