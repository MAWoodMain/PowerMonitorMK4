/**
 * @file     serialflash.h
 * @brief    
 * @version  V0.01
 * @date     31/01/20
 */ 
#ifndef _SERIALFLASH_H_
#define _SERIALFLASH_H_

/***************************** GROUP START ******************************/
//@{     

/**************************** LIB INCLUDES ******************************/
#include <stdbool.h>
#include <stdint.h>
/**************************** USER INCLUDES *****************************/
#include "stm32f4xx_hal.h"
#include "cardTypes.h"
/******************************* DEFINES ********************************/
#define SERIALFLASH_PAGE_SIZE           256U
#define SERIALFLASH_MIN_BLOCK_SIZE      4096U
/******************************** ENUMS *********************************/

/****************************** TYPEDEFS ********************************/

/***************************** STRUCTURES *******************************/

/************************** FUNCTION PROTOTYPES *************************/
bool serialflash_init (cardFilesystem_t* cardFs, cardSpi_t spi, cardSpi_channels_e channel);
bool serialflash_is_ok(cardFilesystem_t cardFs);
bool serialflash_device_ready(cardSpi_t spi, cardSpi_channels_e channel);
bool serialflash_check_unprotected(cardSpi_t spi, cardSpi_channels_e channel);
void serialflash_power_down(cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, bool poweroff);
bool serialflash_write_stream(cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr, uint32_t len, uint8_t* pbuf);
bool serialflash_read_stream(cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr, uint32_t len, uint8_t* pbuf);
bool serialflash_erase_4kblock(cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr);
bool serialflash_blank_check_block (cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr);



/******************************* GLOBALS ********************************/
/* Globals are to be avoided unless justified and documented */
/************************************************************************/

/***************************** GROUP END ********************************/
//@}

#endif