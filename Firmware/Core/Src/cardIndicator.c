/**
  * @file    cardIndicator.c 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */
/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "cardIndicator.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
static void cardIndicator_setRedLed(cardIndicator_t cardIndicator, bool on);
static void cardIndicator_setGreenLed(cardIndicator_t cardIndicator, bool on);
static void cardIndicator_setBlueLed(cardIndicator_t cardIndicator, bool on);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool cardIndicator_init(cardIndicator_t* cardIndicator)
{
    bool retVal = true;
    cardIndicator_setRedLed( *cardIndicator, false );
    cardIndicator_setGreenLed( *cardIndicator, false );
    cardIndicator_setBlueLed( *cardIndicator, false );
    return retVal;
}

void cardIndicator_updateFromState(cardIndicator_t cardIndicator, card_state_e state)
{
    if(state > CARD_STATE_CONFIGURED)
    {
        cardIndicator_setRedLed( cardIndicator, false );
        cardIndicator_setGreenLed( cardIndicator, true );
        cardIndicator_setBlueLed( cardIndicator, false );
    }
    else if( state < CARD_STATE_CONFIGURED)
    {
        cardIndicator_setRedLed( cardIndicator, true );
        cardIndicator_setGreenLed( cardIndicator, false );
        cardIndicator_setBlueLed( cardIndicator, false );
    }
    else
    {
        cardIndicator_setRedLed( cardIndicator, false );
        cardIndicator_setGreenLed( cardIndicator, false );
        cardIndicator_setBlueLed( cardIndicator, true );
    }
}
/*************************** PRIVATE FUNCTIONS **************************/

static void cardIndicator_setRedLed(cardIndicator_t cardIndicator, bool on)
{
    HAL_GPIO_WritePin(cardIndicator.red_port, cardIndicator.red_pin, on? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void cardIndicator_setGreenLed(cardIndicator_t cardIndicator, bool on)
{
    HAL_GPIO_WritePin(cardIndicator.green_port, cardIndicator.green_pin, on? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void cardIndicator_setBlueLed(cardIndicator_t cardIndicator, bool on)
{
    HAL_GPIO_WritePin(cardIndicator.blue_port, cardIndicator.blue_pin, on? GPIO_PIN_RESET : GPIO_PIN_SET);
}