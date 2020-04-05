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
#include "FreeRTOS.h"
#include "queue.h"
#include "cards.h"
/* command function modules */
/******************************* DEFINES ********************************/
#define NO_OPERATIONS 3U
#define MAX_SIGNIFIER_LEN 2U
#define START_SYMBOL '#'
#define RX_BUFFER_SIZE 50U
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
typedef struct
{
    uint8_t* signifier;
    void(*operationImplementations[NO_OPERATIONS])(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args);
} serialInterface_commandFunction_t;

typedef struct
{
    uint8_t msg[RX_BUFFER_SIZE];
} serialInterface_message_t;
/************************** FUNCTION PROTOTYPES *************************/
/* Internal operations */
bool serialInterface_processCommand(uint8_t* rawCommand);
serialInterface_message_t* getFreeMessageBuffer(void);
/* ISR handlers*/
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
        { (uint8_t*) "RR", {
                                  [OPERATION_QUERY]=cards_commandReadRawHandler,
                                  [OPERATION_SET]=serialInterface_unsupportedHandler,
                                  [OPERATION_RESET]=serialInterface_unsupportedHandler,
                          }},
        {NULL}
};

/******************************* VARIABLES ******************************/
volatile bool serialInterface_started = false;
uint8_t serialInterface_rxBuffer[RX_BUFFER_SIZE];
serialInterface_message_t serialInterface_message;
volatile uint16_t serialInterface_rxIdx = 0U;
QueueHandle_t serialInterface_msgQueue;
/*************************** PUBLIC FUNCTIONS ***************************/
bool serialInterface_init(void)
{
    bool retVal = true;

    serialInterface_msgQueue = xQueueCreate( 2, sizeof( serialInterface_message_t ) );

    HAL_UART_RegisterCallback(&huart1,HAL_UART_RX_COMPLETE_CB_ID, serialInterface_rxComplete);

    /* kickoff reception */
    HAL_UART_Receive_IT(&huart1, &serialInterface_rxBuffer[serialInterface_rxIdx], 1U);

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
        while(operation == OPERATION_UNRECOGNISED && operationPosition <= (MAX_SIGNIFIER_LEN + 1) && operationPosition < strlen((char*)rawCommand))
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

        while(command->signifier != NULL && 0U != memcmp(signifier, command->signifier, strlen((char*)command->signifier)))
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
serialInterface_message_t* getFreeMessageBuffer(void)
{
    /* TODO: support multiple message buffers */
    /* blank buffer, then return it*/
    memset(serialInterface_message.msg, 0U, sizeof(serialInterface_message.msg));
    return &serialInterface_message;
}

void serialInterface_rxComplete(UART_HandleTypeDef *huart)
{
    serialInterface_message_t* msgBuffer = NULL;
    BaseType_t xHigherPriorityTaskWoken;
    if(true == serialInterface_started)
    {
        if(serialInterface_rxBuffer[serialInterface_rxIdx] == '\r')
        {
            /* END OF MESSAGE */
            /* Check if message is too short */
            if(serialInterface_rxIdx > 1)
            {
                msgBuffer = getFreeMessageBuffer();
                if(msgBuffer != NULL)
                {
                    if(serialInterface_rxBuffer[serialInterface_rxIdx-1U] == '\n')
                    {
                        memcpy(msgBuffer->msg, serialInterface_rxBuffer, serialInterface_rxIdx-1U);
                    }
                    else
                    {
                        memcpy(msgBuffer->msg, serialInterface_rxBuffer, serialInterface_rxIdx);
                    }
                    xQueueSendFromISR( serialInterface_msgQueue, msgBuffer, &xHigherPriorityTaskWoken );
                    if( xHigherPriorityTaskWoken )
                    {
                        /* Actual macro used here is port specific. */
                        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
                    }
                }
            }
            serialInterface_rxIdx = 0U;
            memset(serialInterface_rxBuffer, 0U, sizeof(serialInterface_rxBuffer));
        }
        else
        {
            serialInterface_rxIdx++;
        }
    }
    else
    {
        if(serialInterface_rxBuffer[serialInterface_rxIdx] == '\r')
        {
            serialInterface_rxIdx = 0U;
            serialInterface_started = true;
        }
    }

    if(serialInterface_rxIdx >= RX_BUFFER_SIZE)
    {
        /* Overflow, wait for this oversize command to finish then re-enable reception */
        serialInterface_rxIdx = 0U;
        memset(serialInterface_rxBuffer, 0U, sizeof(serialInterface_rxBuffer));
        serialInterface_started = false;
    }

    /* start next receive */
    HAL_UART_Receive_IT(huart, &serialInterface_rxBuffer[serialInterface_rxIdx], 1U);

}

void serialInterface_task(void *argument)
{
    serialInterface_message_t incommingMessage;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true)
    {
        if( serialInterface_msgQueue != NULL )
        {
            if(xQueueReceive( serialInterface_msgQueue, &incommingMessage, portMAX_DELAY  ) == pdPASS )
            {
                //printf("d\n\r");
                serialInterface_processCommand( incommingMessage.msg);
            }
        }
        osDelay(100U);
    }
#pragma clang diagnostic pop
}

void serialInterface_versionHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args)
{
    printf("#V=\"A00.10\"\n\r");
}

void serialInterface_unsupportedHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args)
{
    debug_sendf(LEVEL_WARNING, "Unsupported command %c%s%c", START_SYMBOL, signifier, serialInterface_operationSignifiers[operation]);
}