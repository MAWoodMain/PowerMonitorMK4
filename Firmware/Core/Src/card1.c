/**
  * @file    card1.c 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include <ads868x.h>
#include "cmsis_os.h"
#include "card1.h"
#include "main.h"
#include "spi.h"
#include "serialflash.h"
/******************************* DEFINES ********************************/
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
static void setRedLed(bool on);
static void setGreenLed(bool on);
static void setBlueLed(bool on);

static void selectSerialFlash(bool select);
static void selectCS0(bool select);
static void selectCS1(bool select);
static void resetPeripherals();
/******************************* CONSTANTS ******************************/
/* Slot specific mappings */
/* LED CONFIG START*/
static const uint16_t red_pin   = R1_Pin;
static const uint16_t green_pin = G1_Pin;
static const uint16_t blue_pin  = B1_Pin;

static GPIO_TypeDef* red_port     = R1_GPIO_Port;
static GPIO_TypeDef* green_port   = R1_GPIO_Port;
static GPIO_TypeDef* blue_port    = R1_GPIO_Port;
/* LED CONFIG END*/

/* SPI CONFIG START */
SPI_HandleTypeDef spi_handle; /* cannot be set at compile time, see init function */

static const uint16_t cs1_pin   = SSA1_Pin;
static const uint16_t cs2_pin   = SSB1_Pin;
static const uint16_t cs3_pin   = SSC1_Pin;

static GPIO_TypeDef* cs1_port = SSA1_GPIO_Port;
static GPIO_TypeDef* cs2_port = SSB1_GPIO_Port;
static GPIO_TypeDef* cs3_port = SSC1_GPIO_Port;
/* SPI CONFIG END */
serialflash_flashConfig_t card1_flashConfig;
ads868x_config_t card1_vAdcConfig;
ads868x_config_t card1_iAdcConfig;

/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool card1_init(void)
{
    bool retVal = true;
    spi_handle = hspi1;
    setRedLed(false);
    setGreenLed(false);
    setBlueLed(false);
    card1_flashConfig.csFunction = selectSerialFlash;
    card1_flashConfig.spi_handle = spi_handle;
    serialflash_init(&card1_flashConfig);
    if(false == serialflash_is_ok(card1_flashConfig))
    {
        retVal = false;
    }
    card1_vAdcConfig.spi_handle = spi_handle;
    card1_vAdcConfig.csFunction = selectCS0;
    if(false == ads868x_init(&card1_vAdcConfig))
    {
        retVal = false;
    }

    card1_iAdcConfig.spi_handle = spi_handle;
    card1_iAdcConfig.csFunction = selectCS1;
    if(false == ads868x_init(&card1_iAdcConfig))
    {
        retVal = false;
    }
    return retVal;
}

uint16_t card1_readRaw(uint8_t channelId)
{
    uint16_t retVal = 0U;
    switch(channelId)
    {
        case 0:
            retVal = ads868x_readData(card1_vAdcConfig);
            break;
        case 1:
            retVal = ads868x_readData(card1_iAdcConfig);
            break;
    }
    return retVal;
}

/*************************** PRIVATE FUNCTIONS **************************/

static void setRedLed(bool on)
{
    HAL_GPIO_WritePin(red_port, red_pin, on? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void setGreenLed(bool on)
{
    HAL_GPIO_WritePin(green_port, green_pin, on? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void setBlueLed(bool on)
{
    HAL_GPIO_WritePin(blue_port, blue_pin, on? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void selectSerialFlash(bool select)
{
    HAL_GPIO_WritePin(SSB1_GPIO_Port, SSB1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SSC1_GPIO_Port, SSC1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SSA1_GPIO_Port, SSA1_Pin, select?GPIO_PIN_RESET:GPIO_PIN_SET);

}

static void selectCS0(bool select)
{
    HAL_GPIO_WritePin(SSB1_GPIO_Port, SSB1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SSC1_GPIO_Port, SSC1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SSA1_GPIO_Port, SSA1_Pin, select?GPIO_PIN_RESET:GPIO_PIN_SET);

}

static void selectCS1(bool select)
{
    HAL_GPIO_WritePin(SSB1_GPIO_Port, SSB1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SSC1_GPIO_Port, SSC1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SSA1_GPIO_Port, SSA1_Pin, select?GPIO_PIN_RESET:GPIO_PIN_SET);

}

static void resetPeripherals()
{
    HAL_GPIO_WritePin(SSB1_GPIO_Port, SSB1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SSC1_GPIO_Port, SSC1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SSA1_GPIO_Port, SSA1_Pin, GPIO_PIN_RESET);
    osDelay(1);
    HAL_GPIO_WritePin(SSA1_GPIO_Port, SSA1_Pin, GPIO_PIN_SET);

}