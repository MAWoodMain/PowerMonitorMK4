/**
  * @file    serialFlashTypes.h 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#ifndef SERIALFLASHTYPES_H
#define SERIALFLASHTYPES_H

/**************************** LIB INCLUDES ******************************/
#include <stdint.h>
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
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
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //SERIALFLASHTYPES_H