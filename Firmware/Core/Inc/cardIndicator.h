/**
  * @file    cardIndicator.h 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#ifndef CARDINDICATOR_H
#define CARDINDICATOR_H

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "cardTypes.h"
#include "gpio.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool cardIndicator_init(cardIndicator_t* cardIndicator);
void cardIndicator_updateFromState(cardIndicator_t cardIndicator, card_state_e state);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARDINDICATOR_H