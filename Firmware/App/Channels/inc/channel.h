/**
  * @file    channel.h 
  * @brief    
  * @version 1.00   
  * @date    18/04/2020
  */

#ifndef CHANNEL_H
#define CHANNEL_H

/**************************** LIB INCLUDES ******************************/
#include <stdint.h>
#include <stdbool.h>
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
#define CHANNEL_BUFFER_SIZE 128
/******************************** ENUMS *********************************/
typedef enum
{
    CHANNEL_TYPE_NONE,
    CHANNEL_TYPE_VOLTAGE,
    CHANNEL_TYPE_CURRENT,
    CHANNEL_TYPE_TEMPERATURE
} channelType_e;
typedef enum
{
    DATA_TYPE_RAW,
    DATA_TYPE_UINT16,
    DATA_TYPE_UINT32,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE
} dataType_e;
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
typedef struct
{
    channelType_e type;
    uint8_t channelId;
    uint8_t dataBuffer[CHANNEL_BUFFER_SIZE];
    dataType_e dataType;
    bool busy;
} channelTypeDefinition_t;
/************************** FUNCTION PROTOTYPES *************************/
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CHANNEL_H