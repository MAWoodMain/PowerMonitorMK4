/**
  * @file    card1.c 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

/**************************** LIB INCLUDES ******************************/
/**************************** USER INCLUDES *****************************/
#include "card1.h"
#include "main.h"
#include "gpio.h"
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
serialflash_flashConfig card1_flashConfig;

/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/
bool card1_init(void)
{
    spi_handle = hspi1;
    setRedLed(false);
    setGreenLed(false);
    setBlueLed(false);

    card1_flashConfig.csFunction = selectSerialFlash;
    card1_flashConfig.spi_handle = spi_handle;
    serialflash_init(&card1_flashConfig);
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