/**
  * @file    debug.c 
  * @brief    
  * @version 1.00   
  * @date    05/04/2020
  */

/**************************** LIB INCLUDES ******************************/
#include <stdarg.h>
#include <stdint.h>
/**************************** USER INCLUDES *****************************/
#include "debug.h"
#include "serialInterface.h"
#include "printf.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
static const uint8_t debug_level_signifiers[] = {
        [LEVEL_DEBUG]='D',
        [LEVEL_INFO]='I',
        [LEVEL_WARNING]='W',
        [LEVEL_ERROR]='E',
        [LEVEL_CRITICAL]='C'
};
/*************************** PUBLIC FUNCTIONS ***************************/
void debug_sendf(debug_level_e level, const char *format, ...)
{
    printf("%cM%c=",SERIAL_INTERFACE_UNSOLICITED_SYMBOL, debug_level_signifiers[level]);
    va_list argptr;
    va_start(argptr,format);
    vprintf(format,argptr);
    va_end(argptr);
    printf("\n\r");
}
/*************************** PRIVATE FUNCTIONS **************************/ 