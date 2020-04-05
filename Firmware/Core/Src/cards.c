/**
  * @file    cards.c 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

#include "cards.h"
/**************************** LIB INCLUDES ******************************/
#include <stdio.h>
#include <debug.h>
/**************************** USER INCLUDES *****************************/
#include "card1.h"
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

void cards_commandReadRawHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args)
{
    uint32_t cardId = 0;
    uint32_t channelId = 0;
    uint16_t reading = 0U;
    if(2U==sscanf((char*)args, "%lu,%lu", &cardId, &channelId))
    {
        switch (cardId)
        {
            case 1:
                reading = card1_readRaw(channelId);
                printf("#RR=%lu,%lu,%u\n\r", cardId, channelId, reading);
                break;
        }
    }
    else{
        debug_sendf(LEVEL_WARNING, "Unrecognised arguments for read raw");
    }
}
/*************************** PRIVATE FUNCTIONS **************************/ 