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
/******************************* DEFINES ********************************/
#define SERIALFLASH_PAGE_SIZE           256U
#define SERIALFLASH_MIN_BLOCK_SIZE      4096U
/******************************** ENUMS *********************************/

/****************************** TYPEDEFS ********************************/

/***************************** STRUCTURES *******************************/
/**
* This structure holds information about each serial flash device that
* can be handled including its' ID, timings and familiar device strings
*/
typedef struct
{
    const uint32_t  capacity;       /**< Capacity in bytes */
    const uint32_t  erase4KTime;    /**< Time to erase a 4K block in milli seconds */
    const uint8_t   manuId;         /**< Manufacturers ID from Read ID Opcode */
    const uint8_t   maunString[7];  /**< Manufacturer Name as text string */
    const uint16_t  prodId;         /**< Product ID from Read ID Opcode */
    const uint8_t   prodString[14]; /**< Product Name as a text string */
    const uint8_t   features;       /**< Features as a bit pattern see SERIALFLASH_FEATURE_xxxx */
}serialflash_chipid_t;

typedef struct
{
    /* ST HAL spi handle for SPI comms */
    SPI_HandleTypeDef spi_handle;
    /* A function to select/deselect the IC on SPI */
    void(*csFunction)(bool);
    /* Flash type and info */
    serialflash_chipid_t const* device;
} serialflash_flashConfig_t;
/************************** FUNCTION PROTOTYPES *************************/
bool serialflash_init (serialflash_flashConfig_t* flashConfig);
bool serialflash_is_ok(serialflash_flashConfig_t flashConfig);
bool serialflash_device_ready(void);
bool serialflash_check_unprotected(void);
void serialflash_power_down(serialflash_flashConfig_t flashConfig, bool poweroff);
bool serialflash_write_stream(uint32_t addr, uint32_t len, uint8_t* pbuf);
bool serialflash_read_stream(uint32_t addr, uint32_t len, uint8_t* pbuf);
bool serialflash_erase_4kblock(uint32_t addr);
bool serialflash_blank_check_block (uint32_t addr);



/******************************* GLOBALS ********************************/
/* Globals are to be avoided unless justified and documented */
/************************************************************************/

/***************************** GROUP END ********************************/
//@}

#endif