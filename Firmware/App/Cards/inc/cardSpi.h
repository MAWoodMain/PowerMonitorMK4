/**
  * @file    cardSpi.h 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#ifndef CARDSPI_H
#define CARDSPI_H

/**************************** LIB INCLUDES ******************************/
#include <stdbool.h>
#include "cardDefs.h"
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool cardSpi_init(cardSpi_t* spi);
bool cardSpi_selectDevice(cardSpi_t spi, cardSpi_channels_e channel);
void cardSpi_deselectDevice(cardSpi_t spi);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARDSPI_H