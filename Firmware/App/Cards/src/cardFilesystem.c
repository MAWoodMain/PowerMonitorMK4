/**
  * @file    cartFilesystem.c 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "cardFilesystem.h"
#include "serialflash.h"
/******************************* DEFINES ********************************/
#define LOG_PAGE_SIZE       128
#define MAX_FILES           4
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
static s32_t cardFilesystem_read(u32_t addr, u32_t size, u8_t* dst, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel);
static s32_t cardFilesystem_write(u32_t addr, u32_t size, u8_t* src, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel);
static s32_t cardFilesystem_erase(u32_t addr, u32_t size, cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e* channel);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool cardFilesystem_init(cardFilesystem_t* cardFs, cardSpi_t* spi, cardSpi_channels_e channel)
{
    bool retVal = false;
    spiffs_config cfg;
    do
    {
        if(false == serialflash_init(cardFs, *spi, CARD_SPI_FLASH))
        {
            break;
        }

        cfg.hal_read_f = (spiffs_read) cardFilesystem_read;
        cfg.hal_write_f = (spiffs_write) cardFilesystem_write;
        cfg.hal_erase_f = (spiffs_erase) cardFilesystem_erase;
        cfg.spi = spi;
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


FILE* cardFilesystem_fopen(cardFilesystem_t* cardFs, const uint8_t* filename, const uint8_t* mode)
{
    FILE* retVal = NULL;
    spiffs_file fileHandler = 0;
    spiffs_flags modeFlag = 0;


    for (uint8_t fileHanIdx = 0; fileHanIdx < MAX_FILES; fileHanIdx++)
    {
        if (cardFs->fs_handlers[fileHanIdx] == 0)
        {
            /* found a spare local handler now open the file */

            /* convert stdio mode to SPIFFS mode */
            if (mode[0] == 'r')
            {
                /* found read mode */
                modeFlag = SPIFFS_RDONLY;
                if (strrchr( (char const*) mode, '+' ) != NULL)
                {
                    /* read extended update */
                    modeFlag |= SPIFFS_WRONLY;
                }
            }
            else if (mode[0] == 'w')
            {
                /* found write mode */
                modeFlag = SPIFFS_WRONLY | SPIFFS_TRUNC | SPIFFS_CREAT;

                if (strrchr( (char const*) mode, '+' ) != NULL)
                {
                    /* write/update */
                    modeFlag |= SPIFFS_RDONLY;
                }

            }
            else if (mode[0] == 'a')
            {
                /* found append mode */
                modeFlag = SPIFFS_APPEND | SPIFFS_WRONLY;

                if (strrchr( (char const*) mode, '+' ) != NULL)
                {
                    /* read write append */
                    modeFlag |= SPIFFS_RDONLY;
                }
            }
            else
            {
                /* invalid mode flag */
                modeFlag = 0;
            }


            if (modeFlag != 0)
            {
                fileHandler = (uint16_t) SPIFFS_open( &cardFs->fs, (const char*) filename, modeFlag, 0 );
                if (fileHandler >= 0)
                {
                    cardFs->fs_handlers[fileHanIdx] = fileHandler;
                    retVal = &cardFs->fs_handlers[fileHanIdx];
                }
                else
                {
                    /* error opening file */
                    /*debug_send("Error Opening:", false);
                    debug_send((uint8_t*)filename, false);
                    debug_send(" - Mode:", false);
                    debug_send((uint8_t*)mode, false);
                    debug_send(" - Error:", false);
                    debug_send(format_int32_to_ascii((int32_t)fileHandler,0), true);*/
                }
            }

            break;
        }
    }

    return ( retVal );

}

uint32_t cardFilesystem_fwrite(cardFilesystem_t cardFs, const void* ptr, uint32_t size, uint32_t count, FILE* stream)
{
    uint32_t retVal = 0;
    s32_t writeResult = 0;
    s32_t totalBytes = (s32_t) ( size * count );

    if (totalBytes > 0)
    {
        writeResult = SPIFFS_write( &cardFs.fs, *stream, (void*) ptr, totalBytes );

        if (writeResult > 0)
        {
            retVal = (uint32_t) writeResult;
        }
    }

    return ( retVal );
}

uint32_t cardFilesystem_fread(cardFilesystem_t cardFs, void* ptr, uint32_t size, uint32_t count, FILE* stream)
{
    uint32_t retVal = 0;
    s32_t readResult = 0;
    s32_t totalBytes = (s32_t) ( size * count );

    if (totalBytes > 0)
    {
        readResult = SPIFFS_read( &cardFs.fs, *stream, ptr, totalBytes );

        if (readResult > 0)
        {
            retVal = (uint32_t) readResult;
        }
    }

    return ( retVal );
}

int32_t cardFilesystem_fclose(cardFilesystem_t cardFs, FILE* stream)
{
    int32_t retVal = EOF;

    if (SPIFFS_close( &cardFs.fs, *stream ) == SPIFFS_OK)
    {
        retVal = 0;
        *stream = 0;    /* clear the local file handler for reuse */
    }

    return ( retVal );
}

int32_t cardFilesystem_fseek(cardFilesystem_t cardFs, FILE* stream, int32_t offset, uint32_t origin)
{
    int whence;
    int32_t retVal = -1;

    if (origin == SEEK_SET)
    {
        whence = SPIFFS_SEEK_SET;
    }
    else if (origin == SEEK_CUR)
    {
        whence = SPIFFS_SEEK_CUR;
    }
    else
    {
        /* Assume SEEK END */
        whence = SPIFFS_SEEK_END;
    }

    /* A correct seek will return the new file pointer */
    retVal = SPIFFS_lseek( &cardFs.fs, *stream, offset, whence );

    return ( retVal );
}

int32_t cardFilesystem_ftell(cardFilesystem_t cardFs, FILE* stream)
{
    int32_t retVal;

    retVal = SPIFFS_tell( &cardFs.fs, *stream );
    {
        if (retVal < 0)
        {
            retVal = -1;
        }
    }

    return ( retVal );
}

int32_t cardFilesystem_remove(cardFilesystem_t cardFs, const uint8_t* filename)
{
    return (int32_t) ( SPIFFS_remove( &cardFs.fs, (const char*) filename ) );
}

int32_t cardFilesystem_vfs_rename(cardFilesystem_t cardFs, const uint8_t* old, const uint8_t* newPath)
{
    return (int32_t) ( SPIFFS_rename( &cardFs.fs, (const char*) old, (const char*) newPath ) );
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