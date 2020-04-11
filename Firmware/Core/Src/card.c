/**
  * @file    card.c 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "card.h"
#include "debug.h"
#include "crc.h"
#include "serialflash.h"
#include "serialInterface.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
bool card_readConfig(card_t* card);
bool card_configValid(card_t card);
bool card_writeInitialConfig(card_t* card);
bool card_writeConfig(card_t* card);
void card_detect(uint8_t cardId);
/******************************* CONSTANTS ******************************/
static uint8_t* card_configFilename = (uint8_t*) "config.struct";
/******************************* VARIABLES ******************************/

card_t card_cards[] =
        {
                [0] = {
                        .spi = {
                                .spi_base   = SPI1,
                                .cs1_pin    = SSA1_Pin,
                                .cs2_pin    = SSB1_Pin,
                                .cs3_pin    = SSC1_Pin,
                                .cs1_port   = SSA1_GPIO_Port,
                                .cs2_port   = SSB1_GPIO_Port,
                                .cs3_port   = SSC1_GPIO_Port,
                        },
                        .indicator = {
                                .red_pin    = R1_Pin,
                                .green_pin  = G1_Pin,
                                .blue_pin   = B1_Pin,
                                .red_port   = R1_GPIO_Port,
                                .green_port = G1_GPIO_Port,
                                .blue_port  = B1_GPIO_Port,
                        },
                        .state = CARD_STATE_DISCONNECTED,
                        .config = {
                                .type = CARD_TYPE_UNKNOWN,
                                .crc = 0U
                        }
                },
                [1] = {
                        .spi = {
                                .spi_base   = SPI2,
                                .cs1_pin    = SSA2_Pin,
                                .cs2_pin    = SSB2_Pin,
                                .cs3_pin    = SSC2_Pin,
                                .cs1_port   = SSA2_GPIO_Port,
                                .cs2_port   = SSB2_GPIO_Port,
                                .cs3_port   = SSC2_GPIO_Port,
                        },
                        .indicator = {
                                .red_pin    = R2_Pin,
                                .green_pin  = G2_Pin,
                                .blue_pin   = B2_Pin,
                                .red_port   = R2_GPIO_Port,
                                .green_port = G2_GPIO_Port,
                                .blue_port  = B2_GPIO_Port,
                        },
                        .state = CARD_STATE_DISCONNECTED,
                        .config = {
                                .type = CARD_TYPE_UNKNOWN,
                                .crc = 0U
                        }
                },
                [2] = {
                        .spi = {
                                .spi_base   = SPI3,
                                .cs1_pin    = SSA3_Pin,
                                .cs2_pin    = SSB3_Pin,
                                .cs3_pin    = SSC3_Pin,
                                .cs1_port   = SSA3_GPIO_Port,
                                .cs2_port   = SSB3_GPIO_Port,
                                .cs3_port   = SSC3_GPIO_Port,
                        },
                        .indicator = {
                                .red_pin    = R3_Pin,
                                .green_pin  = G3_Pin,
                                .blue_pin   = B3_Pin,
                                .red_port   = R3_GPIO_Port,
                                .green_port = G3_GPIO_Port,
                                .blue_port  = B3_GPIO_Port,
                        },
                        .state = CARD_STATE_DISCONNECTED,
                        .config = {
                                .type = CARD_TYPE_UNKNOWN,
                                .crc = 0U
                        }
                },
                [3] = {
                        .spi = {
                                .spi_base   = SPI4,
                                .cs1_pin    = SSA4_Pin,
                                .cs2_pin    = SSB4_Pin,
                                .cs3_pin    = SSC4_Pin,
                                .cs1_port   = SSA4_GPIO_Port,
                                .cs2_port   = SSB4_GPIO_Port,
                                .cs3_port   = SSC4_GPIO_Port,
                        },
                        .indicator = {
                                .red_pin    = R4_Pin,
                                .green_pin  = G4_Pin,
                                .blue_pin   = B4_Pin,
                                .red_port   = R4_GPIO_Port,
                                .green_port = G4_GPIO_Port,
                                .blue_port  = B4_GPIO_Port,
                        },
                        .state = CARD_STATE_DISCONNECTED,
                        .config = {
                                .type = CARD_TYPE_UNKNOWN,
                                .crc = 0U
                        }
                }
        };

/*************************** PUBLIC FUNCTIONS ***************************/

bool card_init(uint8_t cardId)
{
    card_t* card = &card_cards[cardId];
    bool retVal = true;
    do
    {
        /* Slot side initialisation */
        if(false == cardIndicator_init(&card->indicator))
        {
            retVal = false;
            break;
        }

        if(false == cardSpi_init(&card->spi))
        {
            retVal = false;
            break;
        }

        if(false == card_isPresent(cardId))
        {
            retVal = false;
            break;
        }
    } while(false);
    return retVal;
}

bool card_readConfig(card_t* card)
{
    bool retVal = false;
    FILE* file;

    file = cardFilesystem_fopen( &card->fs, card_configFilename, (const uint8_t*) "r" );
    /* Returns NULL if file does not exist or other FS error */
    if (file != NULL)
    {
        cardFilesystem_fseek( card->fs, file, 0, SEEK_SET );
        cardFilesystem_fread( card->fs, &card->config, sizeof( card_config_t ), 1, file );
        cardFilesystem_fclose( card->fs, file );
        retVal = true;
    }

    return retVal;
}

bool card_configValid(card_t card)
{
    return card.config.crc ==
           HAL_CRC_Calculate( &hcrc,
                              (uint32_t*) &card.config,
                              ( sizeof( card.config ) - sizeof( card.config.crc ) ) / sizeof( uint32_t ) );
}

bool card_writeInitialConfig(card_t* card)
{
    /* TODO: implement proper serial command based configuration */
    card->config.type = CARD_TYPE_VC01;
    return card_writeConfig(card);
}

bool card_writeConfig(card_t* card)
{
    bool retVal = false;
    FILE* file;
    /* Generate CRC */
    card->config.crc =HAL_CRC_Calculate(&hcrc,
                                        (uint32_t*)&card->config, (sizeof(card->config) - sizeof(card->config.crc))/ sizeof(uint32_t));

    file = cardFilesystem_fopen( &card->fs, card_configFilename, (const uint8_t*) "w" );
    /* Returns NULL if file does not exist or other FS error */
    if (file != NULL)
    {
        /* Zero return value means zero bytes written, failure condition */
        if (0 != cardFilesystem_fwrite( card->fs, &card->config, sizeof( card_config_t ), 1U, file ))
        {
            retVal = true;
        }
        cardFilesystem_fclose( card->fs, file );
    }
    return retVal;
}

bool card_isPresent(uint8_t cardId)
{
    card_t* card = &card_cards[cardId];
    card_detect(cardId);
    cardIndicator_updateFromState(card->indicator, card->state);
    return card->state == CARD_STATE_DISCONNECTED;
}

void card_detect(uint8_t cardId)
{
    card_t* card = &card_cards[cardId];
    if(false == serialflash_isPresent(card->spi, CARD_SPI_FLASH))
    {
        if(card->state != CARD_STATE_DISCONNECTED)
        {
            serialInterface_Unsolicitedf(SERIAL_INTERFACE_UNSOLICITED_CARD_CHANGE_TAG, "%d,REMOVED", cardId);
            card->state = CARD_STATE_DISCONNECTED;
            /* blank out config */
            memset(&card->config, 0U, sizeof(card_config_t));
        }
    }
    else
    {
        do
        {
            if(card->state != CARD_STATE_DISCONNECTED)
            {
                /* card was already initialised */
                break;
            }
            serialInterface_Unsolicitedf(SERIAL_INTERFACE_UNSOLICITED_CARD_CHANGE_TAG, "%d,INSERTED", cardId);
            /* On card initialisation */
            if(false == cardFilesystem_init(&card->fs, &card->spi, CARD_SPI_FLASH))
            {
                debug_sendf(LEVEL_WARNING, "card[%d]: Could not initialise filesystem", cardId);
                break;
            }

            if(true == card_readConfig( card ))
            {
                if(true == card_configValid( *card ))
                {
                    debug_sendf(LEVEL_INFO, "card[%d]: Found config, device recognised", cardId);
                    card->state = CARD_STATE_CONFIGURED;
                }
                else
                {
                    debug_sendf(LEVEL_INFO, "card[%d]: Found config, config invalid", cardId);
                    card->state = CARD_STATE_UNIDENTIFIED;
                }
            }
            else
            {
                debug_sendf(LEVEL_INFO, "card[%d]: Missing a config file", cardId);
                if(true == card_writeInitialConfig(card))
                {
                    debug_sendf(LEVEL_INFO, "card[%d]: Generated initial config file", cardId);
                    card->state = CARD_STATE_CONFIGURED;
                }
                else
                {
                    debug_sendf(LEVEL_INFO, "card[%d]: Failed to save config file", cardId);
                    card->state = CARD_STATE_ERROR;
                }
            }
        } while(false);
    }
}
/*************************** PRIVATE FUNCTIONS **************************/