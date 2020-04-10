/**
  * @file    cards.c 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

#include "cards.h"
/**************************** LIB INCLUDES ******************************/
#include <string.h>
/**************************** USER INCLUDES *****************************/
#include "card1.h"
#include "cmsis_os2.h"
#include "printf.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool cards_init(void)
{
    bool retVal = true;

    card1_init();

    return retVal;
}

bool cards_commandReadRawHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr)
{
    uint32_t cardId = 0;
    uint32_t channelId = 0;
    uint16_t reading;
    bool retVal = false;

    do
    {
        if(false == serialInterface_extractUintArg(args, 0U, &cardId, 10U))
        {
            break;
        }
        if(false == serialInterface_extractUintArg(args, 1U, &channelId, 10U))
        {
            break;
        }

        switch (cardId)
        {
            case 1:
                reading = card1_readRaw(channelId);
                snprintf_((char*)replyPtr, REPLY_BUFFER_SIZE, "%lu,%lu,%u", cardId, channelId, reading);
                retVal = true;
                break;
            default:
                break;
        }

    } while(false);

    return retVal;
}

bool cards_commandReadWaveformHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr)
{
    uint32_t cardId = 0;
    uint32_t channelId = 0;
    uint32_t samples = 10U;
    uint32_t period = 0U;
    uint16_t reading;
    bool retVal = false;

    do
    {
        if(false == serialInterface_extractUintArg(args, 0U, &cardId, 10U))
        {
            break;
        }
        if(false == serialInterface_extractUintArg(args, 1U, &channelId, 10U))
        {
            break;
        }
        /* Optional args, do not check return val */
        serialInterface_extractUintArg(args, 2U, &samples, 10U);
        serialInterface_extractUintArg(args, 3U, &period, 10U);

        switch (cardId)
        {
            case 1:
                snprintf_((char*)replyPtr, REPLY_BUFFER_SIZE, "%lu,%lu", cardId, channelId);
                while(samples > 0U)
                {
                    samples--;
                    reading = card1_readRaw(channelId);
                    reading += card1_readRaw(channelId);
                    reading += card1_readRaw(channelId);
                    reading /= 3U;
                    snprintf_((char*)replyPtr + strlen((char*)replyPtr), REPLY_BUFFER_SIZE, ",%u", reading);
                    osDelay(period);
                }
                retVal = true;
                break;
            default:
                break;
        }

    } while(false);

    return retVal;
}
/*************************** PRIVATE FUNCTIONS **************************/ 