/**
  * @file    ads868x.h 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

#ifndef ADS868X_H
#define ADS868X_H

/**************************** LIB INCLUDES ******************************/
#include <stdbool.h>
/**************************** USER INCLUDES *****************************/
#include "stm32f4xx_hal.h"
#include "cardDefs.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool ads868x_init(cardSpi_t spi, cardSpi_channels_e channel);
uint16_t ads868x_readData(cardSpi_t spi, cardSpi_channels_e channel);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //ADS868X_H