/**
  * @file    card.c 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "card.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool card_attemptIdentification(card_t card);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/

card_t card_cards[] =
        {
                [0] = {
                        .spi = {
                                .spi_base = SPI1,
                                .cs1_pin = SSA1_Pin,
                                .cs2_pin = SSB1_Pin,
                                .cs3_pin = SSC1_Pin,
                                .cs1_port = SSA1_GPIO_Port,
                                .cs2_port = SSB1_GPIO_Port,
                                .cs3_port = SSC1_GPIO_Port,
                        },
                        .indicator = {
                                .red_pin = R1_Pin,
                                .green_pin = G1_Pin,
                                .blue_pin = B1_Pin,
                                .red_port = R1_GPIO_Port,
                                .green_port = G1_GPIO_Port,
                                .blue_port = B1_GPIO_Port,
                        },
                        .state = CARD_STATE_UNKNOWN,
                        .type = CARD_TYPE_UNKNOWN
                }
        };

/*************************** PUBLIC FUNCTIONS ***************************/

bool card_init(uint8_t cardId)
{
    card_t card = card_cards[cardId];
    bool retVal = true;
    do
    {
        if(false == cardIndicator_init(&(card.indicator)))
        {
            retVal = false;
        }

        if(false == cardSpi_init(&(card.spi)))
        {
            retVal = false;
        }

        if(false == cardFilesystem_init(&(card.fs), card.spi))
        {
            retVal = false;
        }

    } while(false);
    return retVal;
}

bool card_attemptIdentification(card_t card)
{

}
/*************************** PRIVATE FUNCTIONS **************************/