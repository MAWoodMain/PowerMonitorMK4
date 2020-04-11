/**
  * @file    cardTypes.h
  */

#ifndef CARDTYPES_H
#define CARDTYPES_H

/**************************** LIB INCLUDES ******************************/
#include <stdint.h>
#include <stdbool.h>
/**************************** USER INCLUDES *****************************/
#include "gpio.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "spiffs.h"
#include "serialFlashTypes.h"
/******************************* DEFINES ********************************/
#define FS_LOG_PAGE_SIZE       128
#define FS_MAX_FILES           4
/******************************** ENUMS *********************************/
typedef enum
{
    CARD_STATE_ERROR = -99,
    CARD_STATE_DISABLED = -2,
    CARD_STATE_UNIDENTIFIED = -1,
    CARD_STATE_UNKNOWN = 0,
    CARD_STATE_UNCONFIGURED = 1,
    CARD_STATE_CONFIGURED = 1,
    CARD_STATE_RUNNING = 2
} card_state_e;

typedef enum
{
    CARD_TYPE_UNKNOWN = -1,
    CARD_TYPE_VC01 = 0
} card_type_e;

typedef enum
{
    CARD_SPI_NONE = 0U,
    CARD_SPI_ONE,
    CARD_SPI_TWO,
    CARD_SPI_FLASH,
    CARD_SPI_RST,
} cardSpi_channels_e;
/****************************** TYPEDEFS ********************************/
/***************************** STRUCTURES *******************************/
typedef struct
{
    const uint16_t red_pin;
    const uint16_t green_pin;
    const uint16_t blue_pin;

    GPIO_TypeDef* red_port;
    GPIO_TypeDef* green_port;
    GPIO_TypeDef* blue_port;
} cardIndicator_t;

typedef struct
{
    SPI_TypeDef* spi_base;
    SPI_HandleTypeDef spi_handle;

    SemaphoreHandle_t mutex;

    const uint16_t cs1_pin;
    const uint16_t cs2_pin;
    const uint16_t cs3_pin;

    GPIO_TypeDef* cs1_port;
    GPIO_TypeDef* cs2_port;
    GPIO_TypeDef* cs3_port;

    cardSpi_channels_e selected;
} cardSpi_t;

typedef struct
{
    serialflash_chipid_t const* flashDevice;
    spiffs fs;
    u8_t spiffs_work_buf[FS_LOG_PAGE_SIZE * 2];
    u8_t spiffs_fds[32 * 4];
    u8_t spiffs_cache_buf[( FS_LOG_PAGE_SIZE + 32 ) * 4];
    int16_t fs_handlers[FS_MAX_FILES];
} cardFilesystem_t;

typedef struct
{
    card_type_e type;
    uint32_t crc;
} card_config_t;

typedef struct
{
    cardSpi_t spi;
    cardIndicator_t indicator;
    cardFilesystem_t fs;
    card_state_e state;
    card_config_t config;
} card_t;

typedef struct
{
    bool(*init)(card_t card);
    void(*run)(card_t card);
} card_impl_t;
/************************** FUNCTION PROTOTYPES *************************/
/******************************* GLOBALS ********************************/
/*        Globals are to be avoided unless justified and documented */
/************************************************************************/

#endif //CARDTYPES_H