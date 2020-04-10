/**
  * @file    serialInterface.c 
  * @brief    
  * @version 1.00   
  * @date    05/04/2020
  */

/**************************** LIB INCLUDES ******************************/
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
/**************************** USER INCLUDES *****************************/
#include "serialInterface.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "cards.h"
#include "printf.h"
/* command function modules */
/******************************* DEFINES ********************************/
#define NO_OPERATIONS 3U
#define MAX_SIGNIFIER_LEN 2U
#define START_SYMBOL '#'
#define ARG_DELIMITER ','
#define RX_BUFFER_SIZE 50U
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
typedef struct
{
    uint8_t* signifier;
    bool (* operationImplementations[NO_OPERATIONS])(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args,
                                                     uint8_t* replyPtr);
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
bool serialInterface_versionHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr);
bool serialInterface_unsupportedHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr);
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
        { (uint8_t*) "RW", {
                                   [OPERATION_QUERY]=cards_commandReadWaveformHandler,
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
uint8_t serialInterface_replyBuffer[REPLY_BUFFER_SIZE];
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

bool serialInterface_extractStrArg(uint8_t* argsString, uint8_t argIdx, uint8_t** value)
{
    bool retVal = false;
    uint32_t length = 0U;
    uint8_t* stringPtr = argsString;
    uint8_t* nextStringPtr = argsString;
    uint8_t outputBuffer[20U];

    /* Travel over args string using nextStringPtr */
    while(strlen((char*) nextStringPtr) > 0)
    {
        /* If delimiter found */
        if(nextStringPtr[0] == ARG_DELIMITER)
        {
            /* If argument start was previously found */
            if(argIdx == 0U)
            {
                /* This delimiter is after the target arguments so it signifies the end */
                length = nextStringPtr - stringPtr;
                break;
            }
            else
            {
                /* Acknowledge we have found the end of an argument, (not target argument) */
                argIdx--;
                /* Is this delimiter the one before the target argument */
                if(argIdx == 0U)
                {
                    /* String start is just after the delimiter */
                    stringPtr = nextStringPtr + 1;
                }
            }
        }
        /* Move on to next char */
        nextStringPtr++;
    }

    if(argIdx == 0U)
    {
        /* If argument was at the end of args, length is assumed */
        if(length == 0)
        {
            length = strlen((char*) stringPtr);
        }

        /* Copy using pointer and length into output buffer */
        memcpy(&outputBuffer, stringPtr, length);
        /* Null terminate the string */
        outputBuffer[length] = '\0';
        /* Pass the buffer back as output */
        *value = outputBuffer;
        retVal = true;
    }
    return retVal;
}

bool serialInterface_extractUintArg(uint8_t* argsString, uint8_t argIdx, uint32_t* value, uint8_t radix)
{
    bool retVal = false;
    uint8_t* arg;
    do
    {
        if(false == serialInterface_extractStrArg(argsString, argIdx, &arg))
        {
            /* Could not extract the argument from arguments */
            break;
        }

        *value = strtoul((char*)arg, NULL, radix);
        retVal = true;
    } while(false);
    return retVal;
}
bool serialInterface_extractIntArg(uint8_t* argsString, uint8_t argIdx, int32_t* value, uint8_t radix)
{
    bool retVal = false;
    uint8_t* arg;
    do
    {
        if(false == serialInterface_extractStrArg(argsString, argIdx, &arg))
        {
            /* Could not extract the argument from arguments */
            break;
        }

        *value = strtol((char*)arg, NULL, radix);
        retVal = true;
    } while(false);
    return retVal;
}

bool serialInterface_extractLongDoubleArg(uint8_t* argsString, uint8_t argIdx, long double* value)
{
    bool retVal = false;
    uint8_t* arg;
    do
    {
        if(false == serialInterface_extractStrArg(argsString, argIdx, &arg))
        {
            /* Could not extract the argument from arguments */
            break;
        }

        *value = strtold((char*)arg, NULL);
        retVal = true;
    } while(false);
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
    uint8_t signifier[MAX_SIGNIFIER_LEN + 1U];
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

        /* Prepare reply buffer */
        memset(serialInterface_replyBuffer, 0U, sizeof(serialInterface_replyBuffer));

        /* Call the relevant operation handler */
        if(false == command->operationImplementations[operation](
                signifier,
                operation,
                (strlen((char*)rawCommand) > (operationPosition + 1U)?&rawCommand[operationPosition + 1U]:NULL),
                serialInterface_replyBuffer))
        {
            strcpy((char*)serialInterface_replyBuffer, "ERROR");
        }
        printf("%c%s%c%s\n\r", START_SYMBOL, signifier, serialInterface_operationSignifiers[OPERATION_SET], serialInterface_replyBuffer);

    } while(false);

    return retVal;
}
serialInterface_message_t* getFreeMessageBuffer(void)
{
    /* blank buffer, then return it */
    /* Do not need multiple because the freertos queue allows multiple */
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
    }
#pragma clang diagnostic pop
}

bool serialInterface_versionHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr)
{
    strcpy((char*)replyPtr, "\"A00.10\"");
    return true;
}

bool serialInterface_unsupportedHandler(uint8_t* signifier, serialInterface_operation_e operation, uint8_t* args, uint8_t* replyPtr)
{
    return false;
}