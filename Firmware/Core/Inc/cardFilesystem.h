/**
  * @file    cartFilesystem.h 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#ifndef CARDFILESYSTEM_H
#define CARDFILESYSTEM_H

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "cardTypes.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool cardFilesystem_init(cardFilesystem_t* cardFs, cardSpi_t spi, cardSpi_channels_e channel);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARDFILESYSTEM_H