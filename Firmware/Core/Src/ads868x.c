/**
  * @file    ads868x.c 
  * @brief    
  * @version 1.00   
  * @date    04/04/2020
  */

/**************************** LIB INCLUDES ******************************/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
/**************************** USER INCLUDES *****************************/
#include "ads868x.h"
#include "cmsis_os.h"
/******************************* DEFINES ********************************/
#define ADS868X_OP_CODE_NOP         0x00U;
#define ADS868X_OP_CODE_READ_HWORD  0b1100100;
#define ADS868X_OP_CODE_WRITE_HWORD 0b1101000;
/******************************** ENUMS *********************************/
/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
uint16_t readRegister(ads868x_config_t config, uint16_t address);
void writeRegister(ads868x_config_t config, uint16_t address, uint16_t data);
/******************************* CONSTANTS ******************************/
/******************************* VARIABLES ******************************/
/*************************** PUBLIC FUNCTIONS ***************************/

bool ads868x_init(ads868x_config_t* config)
{
    bool retVal = true;
    /* TODO: Take apply configuration onto ADC */
    return retVal;
}

/*************************** PRIVATE FUNCTIONS **************************/
uint16_t readData(ads868x_config_t config)
{
    uint8_t dataIn[4] = {0};
    uint8_t command[4] = {0};
    uint32_t value = 0U;

    command[2] = ADS868X_OP_CODE_NOP;
    command[2] <<= 1U;

    config.csFunction(true);
    HAL_SPI_TransmitReceive( &config.spi_handle, command, dataIn, 4U, 100U);
    config.csFunction(false);

    value |= dataIn[0];
    value <<= 8U;
    value |= dataIn[1];
    value <<= 8U;
    value |= dataIn[2];
    value <<= 8U;
    value |= dataIn[3];

    return (uint16_t) (value >> 16U);
}

uint16_t readRegister(ads868x_config_t config, uint16_t address)
{
    uint16_t value = 0U;
    uint8_t command[2];
    command[0] = ADS868X_OP_CODE_READ_HWORD;
    command[0] <<= 1U;
    command[1] = address & 0xFFU;
    command[0] |= (address >> 8U) & 1U;

    config.csFunction(true);
    HAL_SPI_Transmit( &config.spi_handle, command, 2U, 100U);
    HAL_SPI_Receive(&config.spi_handle, (uint8_t*) &value, 2U, 100U);
    config.csFunction(false);

    return value;
}

void writeRegister(ads868x_config_t config, uint16_t address, uint16_t data)
{
    uint32_t command = ADS868X_OP_CODE_READ_HWORD;
    command <<= 9U;
    command |= address & 0x1FFU;
    command <<= 16U;
    command |= data;

    config.csFunction(true);
    HAL_SPI_Transmit(&config.spi_handle, (uint8_t*) command, 4U, 100U);
    config.csFunction(false);
}