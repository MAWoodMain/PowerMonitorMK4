/**
  * @file    serialInterface.c 
  * @brief    
  * @version 1.00   
  * @date    05/04/2020
  */

/**************************** LIB INCLUDES ******************************/
#include <string.h>
#include <stdio.h>
#include <ctype.h>
/**************************** USER INCLUDES *****************************/
#include "serialInterface.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "debug.h"
/* command function modules */
/******************************* DEFINES ********************************/
#define NO_OPERATIONS 3U
#define MAX_SIGNIFIER_LEN 2U
#define START_SYMBOL '#'
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
typedef struct
{
    uint8_t* signifier;
    void(*operationImplementations[NO_OPERATIONS])(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args);
} serialInterface_commandFunction_t;
/************************** FUNCTION PROTOTYPES *************************/
/* Internal operations */
bool serialInterface_processCommand(uint8_t* rawCommand);
/* ISR handlers*/
void serialInterface_txComplete(UART_HandleTypeDef *huart);
void serialInterface_rxComplete(UART_HandleTypeDef *huart);
/* Command implementations */
void serialInterface_versionHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args);
void serialInterface_unsupportedHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args);
serialInterface_operation_e serialInterface_recogniseOperation(uint8_t i);
/******************************* CONSTANTS ******************************/
static const uint8_t serialInterface_operationSignifiers[] = {
        [OPERATION_QUERY]='?',
        [OPERATION_SET]='=',
        [OPERATION_RESET]='!'
};

static const serialInterface_commandFunction_t serialInterface_commands[] = {
        { (uint8_t*) "V", {
                                  [OPERATION_QUERY]=serialInterface_versionHandler,
                                  [OPERATION_SET]=serialInterface_unsupportedHandler,
                                  [OPERATION_RESET]=serialInterface_unsupportedHandler,
                                  }},
        {NULL}
};

/******************************* VARIABLES ******************************/
volatile uint32_t cnt = 0U;
volatile bool serialInterface_started = false;
/*************************** PUBLIC FUNCTIONS ***************************/
bool serialInterface_init(void)
{
    bool retVal = true;

    HAL_UART_RegisterCallback(&huart1,HAL_UART_TX_COMPLETE_CB_ID, serialInterface_txComplete);
    HAL_UART_RegisterCallback(&huart1,HAL_UART_RX_COMPLETE_CB_ID, serialInterface_rxComplete);

    serialInterface_processCommand("#v=5");
    serialInterface_processCommand("#v?5");

    return retVal;
}
/*************************** PRIVATE FUNCTIONS **************************/

serialInterface_operation_e serialInterface_recogniseOperation(uint8_t possibleSignifier)
{
    serialInterface_operation_e operation = OPERATION_UNRECOGNISED;
    for(serialInterface_operation_e op = 0U; op < NO_OPERATIONS; op++)
    {
        if(possibleSignifier == serialInterface_operationSignifiers[op])
        {
            operation = op;
            break;
        }
    }
    return operation;
}

bool serialInterface_processCommand(uint8_t* rawCommand)
{
    bool retVal = true;
    uint16_t pos;
    uint16_t operationPosition = 1U;
    uint8_t signifier[MAX_SIGNIFIER_LEN];
    serialInterface_operation_e operation = OPERATION_UNRECOGNISED;
    serialInterface_commandFunction_t const* command = serialInterface_commands;

    /* Do while to allow breaking on problems */
    do{

        if(rawCommand[0] != START_SYMBOL)
        {
            retVal = false;
            break;
        }
        /* Search for an operation signifier, if one is not found by pos=MAX_SIGNIFIER_LEN+1  (+1 for START_SYMBOL) then it is not valid */
        while(operation == OPERATION_UNRECOGNISED && operationPosition <= (MAX_SIGNIFIER_LEN + 1))
        {
            operationPosition++;
            operation = serialInterface_recogniseOperation(rawCommand[operationPosition]);
        }

        if(operation == OPERATION_UNRECOGNISED)
        {
            retVal = false;
            break;
        }
        memset(signifier, 0U, sizeof(signifier));

        for(pos = 0U; pos < operationPosition - 1U; pos++)
        {
            signifier[pos] = toupper(rawCommand[1U + pos]);
        }

        while(command->signifier != NULL && 0U != memcmp(signifier, command->signifier, operationPosition))
        {
            command++;
        }
        if(command->signifier == NULL)
        {
            /* No matching signature in command set */
            retVal = false;
            break;
        }
        /* Call the relevant operation handler */
        if(strlen((char*)rawCommand) > (operationPosition + 1U))
        {
            /* Has arguments */
            command->operationImplementations[operation](signifier, operation, &rawCommand[operationPosition + 1U]);
        }
        else
        {
            /* Does not have arguments */
            command->operationImplementations[operation](signifier, operation, NULL);
        }

    } while(false);

    return retVal;
}


void serialInterface_txComplete(UART_HandleTypeDef *huart)
{
    cnt++;
}

void serialInterface_rxComplete(UART_HandleTypeDef *huart)
{

}

void serialInterface_task(void *argument)
{
    while(true)
    {
        osDelay(1000U);
    }
}

void serialInterface_versionHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args)
{
    printf("#V=\"A00.10\"\n\r");
}

void serialInterface_unsupportedHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args)
{
    debug_sendf("Unsupported command %c%s%c\n\r", START_SYMBOL, signifier, serialInterface_operationSignifiers[operation]);
}