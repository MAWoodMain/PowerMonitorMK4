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
#include "cardDefs.h"
#include "serialInterface.h"
/******************************* DEFINES ********************************/
#define NO_OF_CARDS 4U
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool card_init(uint8_t cardId);
void card_process(uint8_t cardId);
bool card_isPresent(uint8_t cardId);
bool card_queryHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARD_H