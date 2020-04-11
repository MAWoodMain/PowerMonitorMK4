/**
  * @file    cartFilesystem.c 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include <debug.h>
#include "cardFilesystem.h"
#include "serialflash.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
static s32_t cardFilesystem_read(u32_t addr, u32_t size, u8_t* dst, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel);
static s32_t cardFilesystem_write(u32_t addr, u32_t size, u8_t* src, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel);
static s32_t cardFilesystem_erase(u32_t addr, u32_t size, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool cardFilesystem_init(cardFilesystem_t* cardFs, cardSpi_t spi, cardSpi_channels_e channel)
{
    bool retVal = false;
    spiffs_config cfg;
    do
    {
        if(false == serialflash_init(cardFs, spi, CARD_SPI_FLASH))
        {
            break;
        }

        cfg.hal_read_f = (spiffs_read) cardFilesystem_read;
        cfg.hal_write_f = (spiffs_write) cardFilesystem_write;
        cfg.hal_erase_f = (spiffs_erase) cardFilesystem_erase;
        cfg.spi = &spi;
        cfg.channel = &channel;
        cfg.cardFs = cardFs;

        int res = SPIFFS_mount( &(cardFs->fs),
                                &cfg,
                                cardFs->spiffs_work_buf,
                                cardFs->spiffs_fds,
                                sizeof( cardFs->spiffs_fds ),
                                cardFs->spiffs_cache_buf,
                                sizeof( cardFs->spiffs_cache_buf ),
                                0 );

        if (SPIFFS_OK == res)
        {
            retVal = true;
        }
        else if (SPIFFS_ERR_NOT_A_FS == res)
        {
            /* need to format the flash */
            SPIFFS_unmount( &(cardFs->fs) );

            SPIFFS_format( &(cardFs->fs) );

            res = SPIFFS_mount( &(cardFs->fs),
                                &cfg,
                                cardFs->spiffs_work_buf,
                                cardFs->spiffs_fds,
                                sizeof( cardFs->spiffs_fds ),
                                cardFs->spiffs_cache_buf,
                                sizeof( cardFs->spiffs_cache_buf ),
                                0 );

            if (SPIFFS_OK == res)
            {
                retVal = true;

            }
        }
    } while(false);
    return retVal;
}
/*************************** PRIVATE FUNCTIONS **************************/
static s32_t cardFilesystem_read(u32_t addr, u32_t size, u8_t* dst, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel)
{
    s32_t retVal = SPIFFS_ERR_NOT_READABLE;
    cardFilesystem_t _cardFs = *cardFs;
    cardSpi_t _spi = *spi;
    cardSpi_channels_e _channel = *channel;
    if (serialflash_read_stream(_cardFs, _spi, _channel, (uint32_t) addr, (uint32_t) size, dst ) == true)
    {
        retVal = SPIFFS_OK;
    }
    //debug_sendf(LEVEL_DEBUG, "READ 0x%.8X -> 0x%.4X", addr, *((uint16_t*)dst));

    return retVal;
}

static s32_t cardFilesystem_write(u32_t addr, u32_t size, u8_t* src, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel)
{
    s32_t retVal = SPIFFS_ERR_NOT_WRITABLE;
    cardFilesystem_t _cardFs = *cardFs;
    cardSpi_t _spi = *spi;
    cardSpi_channels_e _channel = *channel;
    //debug_sendf(LEVEL_DEBUG, "WRITE 0x%.8X -> 0x%.4X", addr, *((uint16_t*)src));
    if (serialflash_write_stream(_cardFs, _spi, _channel, (uint32_t) addr, (uint32_t) size, src ) == true)
    {
        retVal = SPIFFS_OK;
    }

    return retVal;
}

static s32_t cardFilesystem_erase(u32_t addr, u32_t size, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel)
{
    s32_t retVal = SPIFFS_ERR_NOT_WRITABLE;
    cardFilesystem_t _cardFs = *cardFs;
    cardSpi_t _spi = *spi;
    cardSpi_channels_e _channel = *channel;

    if (size == SERIALFLASH_MIN_BLOCK_SIZE)
    {
        if (serialflash_erase_4kblock(_cardFs, _spi, _channel, (uint32_t) addr ) == true)
        {
            retVal = SPIFFS_OK;
        }
    }

    return retVal;
}