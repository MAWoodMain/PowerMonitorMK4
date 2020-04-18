/**
  * @file    cartFilesystem.h 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#ifndef CARDFILESYSTEM_H
#define CARDFILESYSTEM_H

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "cardDefs.h"
/******************************* DEFINES ********************************/
#ifdef EOF
#undef EOF
#endif
#define EOF             (-1)


#ifdef SEEK_SET
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#endif
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2
/******************************** ENUMS *********************************/
/****************************** TYPEDEFS ********************************/
typedef int16_t FILE;
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool cardFilesystem_init(cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e channel);
FILE* cardFilesystem_fopen(cardFilesystem_t* cardFs, const uint8_t* filename, const uint8_t* mode);
uint32_t cardFilesystem_fwrite(cardFilesystem_t cardFs, const void* ptr, uint32_t size, uint32_t count, FILE* stream);
uint32_t cardFilesystem_fread(cardFilesystem_t cardFs, void* ptr, uint32_t size, uint32_t count, FILE* stream);
int32_t cardFilesystem_fclose(cardFilesystem_t cardFs, FILE* stream);
int32_t cardFilesystem_fseek(cardFilesystem_t cardFs, FILE* stream, int32_t offset, uint32_t origin);
int32_t cardFilesystem_remove(cardFilesystem_t cardFs, const uint8_t* filename);
int32_t cardFilesystem_ftell(cardFilesystem_t cardFs, FILE* stream);
int32_t cardFilesystem_vfs_rename(cardFilesystem_t cardFs, const uint8_t* old, const uint8_t* newPath);
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARDFILESYSTEM_H