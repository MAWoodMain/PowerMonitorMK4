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
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
typedef struct
{
    /* ST HAL spi handle for SPI comms */
    SPI_HandleTypeDef spi_handle;
    /* A function to select/deselect the IC on SPI */
    void(*csFunction)(bool);
} ads868x_config_t;
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool ads868x_init(ads868x_config_t* config);
uint16_t readData(ads868x_config_t config);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //ADS868X_H