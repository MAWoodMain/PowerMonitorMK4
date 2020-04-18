/**
  * @file    debug.h 
  * @brief    
  * @version 1.00   
  * @date    05/04/2020
  */

#ifndef DEBUG_H
#define DEBUG_H

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
typedef enum
{
    LEVEL_DEBUG = 0U,
    LEVEL_INFO = 1U,
    LEVEL_WARNING = 2U,
    LEVEL_ERROR = 3U,
    LEVEL_CRITICAL = 4U
} debug_level_e;
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
void debug_init(void);
void debug_sendf(debug_level_e level, const char *format, ...);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //DEBUG_H