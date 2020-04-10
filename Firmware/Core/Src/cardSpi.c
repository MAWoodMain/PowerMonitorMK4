/**
  * @file    cardSpi.c 
  * @brief    
  * @version 1.00   
  * @date    10/04/2020
  */

#include "cardSpi.h"
/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
/******************************* DEFINES ********************************/
#define CARD_SPI_TAKE_SEMAPHORE_TIMEOUT 100U
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
SPI_HandleTypeDef cardSpi_getSpiHandle(SPI_TypeDef * spiBase);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool cardSpi_init(cardSpi_t* spi)
{
    bool retVal = false;

    spi->mutex = xSemaphoreCreateMutex();
    if(spi->mutex != NULL)
    {
        retVal = true;
    }
    spi->spi_handle = cardSpi_getSpiHandle(spi->spi_base);

    return retVal;
}

void cardSpi_selectChipSelect(cardSpi_t spi, cardSpi_channels_e channel, bool select)
{
    switch(channel)
    {
        case CARD_SPI_ONE:
            HAL_GPIO_WritePin(spi.cs2_port, spi.cs2_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(spi.cs3_port, spi.cs3_pin, GPIO_PIN_RESET);
            break;
        case CARD_SPI_TWO:
            HAL_GPIO_WritePin(spi.cs2_port, spi.cs2_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(spi.cs3_port, spi.cs3_pin, GPIO_PIN_RESET);
            break;
        case CARD_SPI_FLASH:
            HAL_GPIO_WritePin(spi.cs2_port, spi.cs2_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(spi.cs3_port, spi.cs3_pin, GPIO_PIN_SET);
            break;
        case CARD_SPI_RST:
            HAL_GPIO_WritePin(spi.cs2_port, spi.cs2_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(spi.cs3_port, spi.cs3_pin, GPIO_PIN_SET);
            break;
        case CARD_SPI_NONE:
        default:
            /* Do nothing */
            break;
    }
    HAL_GPIO_WritePin(spi.cs1_port, spi.cs1_pin, select?GPIO_PIN_RESET:GPIO_PIN_SET);
}

bool cardSpi_selectDevice(cardSpi_t spi, cardSpi_channels_e channel)
{
    bool retVal = false;

    if(pdTRUE == xSemaphoreTake(spi.mutex, CARD_SPI_TAKE_SEMAPHORE_TIMEOUT))
    {
        cardSpi_selectChipSelect(spi, channel, true);
        retVal = true;
    }

    return retVal;
}

void cardSpi_deselectDevice(cardSpi_t spi)
{
    cardSpi_selectChipSelect(spi, CARD_SPI_NONE, true);
    xSemaphoreGive(spi.mutex);
}

/*************************** PRIVATE FUNCTIONS **************************/
SPI_HandleTypeDef cardSpi_getSpiHandle(SPI_TypeDef* spiBase)
{
    if(SPI1 == spiBase)
    {
        return hspi1;
    }
    if(SPI2 == spiBase)
    {
        return hspi2;
    }
    if(SPI3 == spiBase)
    {
        return hspi3;
    }
    if(SPI4 == spiBase)
    {
        return hspi4;
    }

    return hspi1;
}