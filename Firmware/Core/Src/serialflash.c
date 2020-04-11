/*!
 * @file        serialflash.c    
 * @brief    
 * @version     1.00 
 * @date        31/01/20
 */ 

/**************************** LIB INCLUDES ******************************/
#include <cmsis_os2.h>
/**************************** USER INCLUDES *****************************/
#include "serialflash.h"
#include "main.h"
#include "cardSpi.h"
/***************************** GROUP START ******************************/
//@{
/******************************* DEFINES ********************************/
#define SERIALFLASH_DEBUG

#define SERIALFLASH_NONE            0U              /* device discovery not done */
/* 1-254 are the index's into the ChipID table below */
#define SERIALFLASH_FAIL            255U            /* could not find a valid device */

/* opcode */
#define SERIALFLASH_OPCODE_ERASE_4KBLOCK        0x20U
#define SERIALFLASH_OPCODE_ERASE_32KBLOCK       0x52U
#define SERIALFLASH_OPCODE_ERASE_64KBLOCK       0xD8U
#define SERIALFLASH_OPCODE_ERASE_CHIP           0x60U
#define SERIALFLASH_OPCODE_ERASE_C7             0xC7U           /* Some devices use C7 for erase */
#define SERIALFLASH_OPCODE_WRITE_ENABLE         0x06U
#define SERIALFLASH_OPCODE_BYTE_PROGRAM         0x02U
#define SERIALFLASH_OPCODE_READ_STATUS          0x05U
#define SERIALFLASH_OPCODE_WRITE_STATUS         0x01U
#define SERIALFLASH_OPCODE_READ_ARRAY           0x0BU           /* use high frequency */    
#define SERIALFLASH_OPCODE_READ_ID              0x9FU
#define SERIALFLASH_OPCODE_DEEP_SLEEP           0xB9U
#define SERIALFLASH_OPCODE_RESUME               0xABU


/* bits in status register */
#define SERIALFLASH_STATUS_BSY                  0x01U           /* Device is busy with an internal operation */
#define SERIALFLASH_STATUS_WEL                  0x02U           /* Device is write enabled */
#define SERIALFLASH_STATUS_SWP_NONE             0U              /* All sectors are software unprotected */
#define SERIALFLASH_STATUS_SWP_SOME             0x04U           /* Some sectors are software protected*/
#define SERIALFLASH_STATUS_SWP_RES              0x08U           /* Reserved for future use */
#define SERIALFLASH_STATUS_SWP_ALL              0x0CU           /* All sectors are software protected */
#define SERIALFLASH_STATUS_WPP                  0x10U           /* WP is deasserted */
#define SERIALFLASH_STATUS_EPE                  0x20U           /*  Erase or program error detected */
#define SERIALFLASH_STATUS_SPM                  0x40U           /* Sequential Programming Mode entered */
#define SERIALFLASH_STATUS_SPRL                 0x80U           /* Sector Protection Registers are locked */


#define SERIALFLASH_PAGE_WRITE_TIME             20U     /* actually 5ms but allow a little extra */
#define SERIALFLASH_PAGES_PER_BLOCK             (SERIALFLASH_MIN_BLOCK_SIZE / SERIALFLASH_PAGE_SIZE)

#define SERIALFLASH_FEATURE_EPE                 0x01U           /* has EPE feature in status register */
#define SERIALFLASH_FEATURE_POWERDOWN           0x02U           /* has low power feature */
#define SERIALFLASH_FEATURE_C7ERASE             0x04U           /* Command C7h for chip erase */

/* macro to recover Features Settings easily */
/* Note: serialflash_Device is always + 1 to avoid using 0 index */
#define SERIALFLASH_ERASE32KTIME    (SERIALFLASH_ERASE4KTIME * 3U)
#define SERIALFLASH_ERASE64KTIME    (SERIALFLASH_ERASE4KTIME * 5U)
#define SERIALFLASH_ERASECHIPTIME   ((SERIALFLASH_CAPACITY / 0x00100000U) * 15000U)       /* Approx worst case is 15S/MB */
/******************************** ENUMS *********************************/

/***************************** STRUCTURES *******************************/
/************************** FUNCTION PROTOTYPES *************************/
static uint32_t serialflash_read_id(cardSpi_t spi, cardSpi_channels_e channel);
static uint8_t serialflash_read_status(cardSpi_t spi, cardSpi_channels_e channel);
static bool serialflash_wait_busy (cardSpi_t spi, cardSpi_channels_e channel, uint32_t maxtime);
/******************************* CONSTANTS ******************************/
/**
* This table holds all the serial flash devices that can be fitted or accepted
* A Capacity of 0 indicates end of table and also no device fitted 
*/
static const serialflash_chipid_t serialflash_chipId[]={
  {2097152U, 200U,0x1FU, "Atmel",  0x4602U,"AT25DF161",   SERIALFLASH_FEATURE_EPE|SERIALFLASH_FEATURE_POWERDOWN},
  {2097152U, 200U,0x1FU, "Atmel",  0x4601U,"AT25DF161A",  SERIALFLASH_FEATURE_EPE|SERIALFLASH_FEATURE_POWERDOWN},
  {2097152U, 400U,0xEFU, "W'bond", 0x4015U,"W25Q16CV",    SERIALFLASH_FEATURE_POWERDOWN},
  {4194304U, 400U,0xEFU, "W'bond", 0x4016U,"W25Q32CV",    SERIALFLASH_FEATURE_POWERDOWN},
  {1048576U, 300U,0xC8U, "Giga",   0x4015U,"GD25Q16B",    SERIALFLASH_FEATURE_POWERDOWN},
  {0U      , 0U  ,0U   , ""     ,  0U     ,""        ,0U     }
};
    
/******************************* VARIABLES ******************************/





/*************************** PUBLIC FUNCTIONS ***************************/



/*!
 *
 * @brief        Initilise system and discover which device is fitted
 *        
 * @param[in]    csfn Pointer to alternative CS Function
 *
 * @return       bool true found a valid device
 *
 * @note         
 *
 */
bool serialflash_init (cardFilesystem_t* cardFs, cardSpi_t spi, cardSpi_channels_e channel)
{
    uint8_t manId;
    uint16_t prodId;
    bool retVal=false;
    
    /* Try and wake up a sleeping device */
    serialflash_read_status(spi, channel);
    serialflash_power_down(*cardFs, spi, channel, false);           /* power on the device */
      
    /* Read the device type and find a match */
    uint32_t chipId = serialflash_read_id(spi, channel);

    cardFs->flashDevice = serialflash_chipId;
    
    /* Search the device table for matching manufacture and productID */
    while (cardFs->flashDevice->capacity != 0)
    {
        manId = (uint8_t)(chipId >> 16U);             /* retrieve manufacturer's id */
        prodId = (uint16_t)(chipId & 0x0000FFFFU);    /* retrieve product id and mask off unwanted bits */
        
        if ((cardFs->flashDevice->manuId == manId) &&             /* match manufacturer's id */
           (cardFs->flashDevice->prodId == prodId))             /* match product id */
        {
          retVal = true;
          break;
        }

        cardFs->flashDevice++;
    }
    return (retVal);
}
/*!
 *
 * @brief        Quick check to see if the serial flash is ok
 *        
 * @param[in]    None
 *
 * @return       bool, true=flash is present and ok
 *
 * @note         
 *
 */
bool serialflash_is_ok (cardFilesystem_t cardFs)
{
    bool retVal = false;
  
    if (cardFs.flashDevice->capacity != 0)
    {
        retVal = true;
    }
  
    return (retVal);
}

/*!
 *
 * @brief        checks to see if device is ready
 *        
 * @param[in]    None
 *
 * @return       bool, true=device ready   
 *
 * @note         
 *
 */ 
bool serialflash_device_ready (cardSpi_t spi, cardSpi_channels_e channel)
{
    bool retVal = false;
  
    if ((serialflash_read_status(spi, channel) & SERIALFLASH_STATUS_BSY ) == 0U)
    {
        retVal = true;
    }
  
    return (retVal);
}

/*!
 *
 * @brief        Checks the device is unprotected i.e. can write and erase
 *        
 * @param[in]    None
 *
 * @return       bool, true device is unprotected
 *
 * @note         
 *
 */ 
bool serialflash_check_unprotected (cardSpi_t spi, cardSpi_channels_e channel)
{
    bool retVal = false;
    uint8_t statusReg;
  
    statusReg = serialflash_read_status(spi, channel);
    if ((statusReg & SERIALFLASH_STATUS_SWP_ALL) == 0U)
    {
      retVal = true;
    }
    else
    {
      retVal = false;
    }
    
    return retVal;
}

/*!
 *
 * @brief        Read a random stream of data from the flash 
 *        
 * @param[in]    addr    address of where to start reading
 * @param[in]    len length of stream
 * @param[in]    pbuf    pointer to output buffer
 *
 * @param[out]   pbuf    updated contents of the read
 *
 * @return       bool true/false if device can be read and address in range 
 *
 * @note         
 *
 */ 
bool serialflash_read_stream (cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr, uint32_t len, uint8_t* pbuf)
{
    bool retVal = false;
    uint8_t txBuff[5];
    
    /* check device is found */
    if ((cardFs.flashDevice->capacity != 0U) &&
       (len != 0U)                          &&
       (pbuf != NULL))
    {
      /* check all address's are in range */
        if ((addr + len - 1U) < (cardFs.flashDevice->capacity))
        {
            if (serialflash_wait_busy(spi, channel, SERIALFLASH_PAGE_WRITE_TIME) == true)
            {
                txBuff[0] = SERIALFLASH_OPCODE_READ_ARRAY;
                txBuff[1] = (uint8_t)(addr>>16);
                txBuff[2] = (uint8_t)(addr>>8);
                txBuff[3] = (uint8_t)addr;
                txBuff[4] = 0U;

                cardSpi_selectDevice(spi, channel);
                HAL_SPI_Transmit(&spi.spi_handle, txBuff, 5U, 100U);
                HAL_SPI_Receive(&spi.spi_handle, pbuf, len, 100U);
                cardSpi_deselectDevice(spi);
                
                /* need to reset the read buffer */
                retVal = true;
            }
        }
    }
    
    return (retVal);
}


/*!
 *
 * @brief        Write a random stream of data to the flash  
 *        
 * @param[in]    addr    address of where to start writing
 * @param[in]    len     length of stream
 * @param[in]    pbuf    pointer to input buffer
 *
 * @return       bool true/false if device can be written and address in range 
 *
 * @note         The block must be erased prior to this call
 *               The data must reside in a single page
 *               The sector must not be protected  
 *
 */
bool serialflash_write_stream (cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr, uint32_t len, uint8_t* pbuf)
{
  
    bool retVal = false;
    uint8_t txBuff[4];
    
    /* basic sanity checks first */
    if ((serialflash_check_unprotected(spi, channel) == true)    &&
       (cardFs.flashDevice->capacity != 0U)                 &&
       (len <= SERIALFLASH_PAGE_SIZE)               &&
       (len != 0U)                                  &&
       (pbuf != NULL)                               &&
       ((addr + len - 1U) < cardFs.flashDevice->capacity))
       
    {
        /* now check that all the bytes written fit in a single page */
        if((addr / SERIALFLASH_PAGE_SIZE) == ( (addr + len - 1U) / SERIALFLASH_PAGE_SIZE))
        {
            /* wait for ready */
            if(serialflash_wait_busy(spi, channel, SERIALFLASH_PAGE_WRITE_TIME) == true)
            {

                txBuff[0] = SERIALFLASH_OPCODE_WRITE_ENABLE;
                cardSpi_selectDevice(spi, channel);

                HAL_SPI_Transmit(&spi.spi_handle, txBuff, 1U, 100U);

                cardSpi_deselectDevice(spi);

                txBuff[0] = SERIALFLASH_OPCODE_BYTE_PROGRAM;
                txBuff[1] = (uint8_t)(addr>>16);
                txBuff[2] = (uint8_t)(addr>>8);
                txBuff[3] = (uint8_t)addr;

                cardSpi_selectDevice(spi, channel);

                HAL_SPI_Transmit(&spi.spi_handle, txBuff, 4U, 100U);
                HAL_SPI_Transmit(&spi.spi_handle, pbuf, len, 100U);

                cardSpi_deselectDevice(spi);
                
                if(serialflash_wait_busy(spi, channel, SERIALFLASH_PAGE_WRITE_TIME) == true)
                {
                    if((cardFs.flashDevice->features & SERIALFLASH_FEATURE_EPE) != 0U)
                    {
                        /* check EPE to make sure everything is ok */
                        if((serialflash_read_status(spi, channel) & SERIALFLASH_STATUS_EPE) == 0U)
                        {
                            retVal = true;                    
                        }
                    }
                    else
                    {
                        retVal = true;
                    }
                }
            }
        }
    }
    
    return (retVal);
}
/*!
 *
 * @brief        Places or release device from deep sleep
 *        
 * @param[in]    poweroff true=power down, false = power up
 *
 * @return       None
 *
 * @note         if powering up ignores safeguards 
 *
 */ 
void serialflash_power_down(cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, bool poweroff)
{
    bool okToAction = true;
    uint8_t txBuff[1];
    
    
    /* trying to power off an unknown device */
    if ((poweroff == true) && (cardFs.flashDevice->capacity == 0))
    {
        okToAction = false;
    }
    
    /* trying to power off a device which has no low power mode */
    if ((poweroff == true) && (cardFs.flashDevice->features & SERIALFLASH_FEATURE_POWERDOWN) )
    {
        /* tried to power off after writing a page but failed */
        if ((serialflash_wait_busy(spi, channel, SERIALFLASH_PAGE_WRITE_TIME) == false) && (poweroff == true))
        {
            okToAction = false;
        }
    }
    
    /* can power on or off the device */
    if (okToAction == true)
    {
        
        if(poweroff == true)
        {
          
          txBuff[0] = SERIALFLASH_OPCODE_DEEP_SLEEP;
        }
        else
        {
          txBuff[0] = SERIALFLASH_OPCODE_RESUME;
        }


        cardSpi_selectDevice(spi, channel);
        HAL_SPI_Transmit(&spi.spi_handle, txBuff, 1U, 100U);
        cardSpi_deselectDevice(spi);
    }
    
    if (poweroff == false)
    {
        /* needs some time to allow device to restart.. typically 30uS */
        volatile uint32_t dly = 1000U;
        while(--dly)
        {
            ;  
        }
    }
    
}

/*!
 *
 * @brief        Erases an entrire 4K Sector
 *        
 * @param[in]    addr    address inside of block
 *
 * @return       bool    true if block erased ok
 *
 * @note         This function will wait until a 4K block erased.
 *               This function could therefore hold up execution for 200mS
 *               Sector must be unprotected first.
 *
 */

bool serialflash_erase_4kblock (cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr)
{
    bool retVal=false;
    uint8_t txBuff[4];
    
    /* basic sanity checks first */
    if ((serialflash_check_unprotected(spi, channel) == true)  &&
        (cardFs.flashDevice->capacity != 0U)               &&
        (addr < cardFs.flashDevice->capacity))
    { 
        /* wait for ready */
        if(serialflash_wait_busy(spi, channel, SERIALFLASH_PAGE_WRITE_TIME) == true)
        {

            txBuff[0] = SERIALFLASH_OPCODE_WRITE_ENABLE;
            cardSpi_selectDevice(spi, channel);

            HAL_SPI_Transmit(&spi.spi_handle, txBuff, 1U, 100U);

            cardSpi_deselectDevice(spi);

            txBuff[0] = SERIALFLASH_OPCODE_ERASE_4KBLOCK;
            txBuff[1] = (uint8_t)(addr>>16);
            txBuff[2] = (uint8_t)(addr>>8);
            txBuff[3] = (uint8_t)addr;

            cardSpi_selectDevice(spi, channel);

            HAL_SPI_Transmit(&spi.spi_handle, txBuff, 4U, 100U);

            cardSpi_deselectDevice(spi);
              
            /* wait while block erases */
            if(serialflash_wait_busy(spi, channel, cardFs.flashDevice->erase4KTime) == true)
            {
                if((cardFs.flashDevice->features & SERIALFLASH_FEATURE_EPE) != 0U)
                {
                    /* check EPE to make sure everything is ok */
                    if((serialflash_read_status(spi, channel) & SERIALFLASH_STATUS_EPE) == 0U)
                    {
                        retVal = true;                  
                    }
                }
                else
                {
                    retVal = true;
                }               
            }
        }
    }
    return (retVal);
}

/*****************************************************************************//*!
*
* @brief        Erase an entire chip
*        
* @param[in]    wait    true=hold up execution until complete
*
* @return       bool    true=chip erased ok or command accepted
*
* @note         This function could therefore hold up execution for
*                   60S!! If waiting make sure watchdog is disabled
*                   Sector must be unprotected first.
*                   Use serialflash_DeviceReady to check when erase has completed
*                   if not using wait.
*
*****************************************************************************/
bool serialflash_EraseChip(cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, bool wait)
{
    bool retVal = false;
    uint8_t txBuff[1];
    
    /* basic sanity checks first */
    if ((serialflash_check_unprotected(spi, channel) == true) &&
        (cardFs.flashDevice->capacity != 0U))
    { 
        /* wait for ready */
        if(serialflash_wait_busy(spi, channel, SERIALFLASH_PAGE_WRITE_TIME) == true)
        {

            txBuff[0] = SERIALFLASH_OPCODE_WRITE_ENABLE;
            cardSpi_selectDevice(spi, channel);

            HAL_SPI_Transmit(&spi.spi_handle, txBuff, 1U, 100U);

            cardSpi_deselectDevice(spi);

            if(cardFs.flashDevice->features & SERIALFLASH_FEATURE_C7ERASE)
            {
                txBuff[0] = SERIALFLASH_OPCODE_ERASE_C7;
            }
            else
            {
                txBuff[0] = SERIALFLASH_OPCODE_ERASE_CHIP;
            }

            cardSpi_selectDevice(spi, channel);

            HAL_SPI_Transmit(&spi.spi_handle, txBuff, 1U, 100U);

            cardSpi_deselectDevice(spi);
              
            /* wait while chip erases */
            if (true == wait)
            {
                if (serialflash_wait_busy(spi, channel, (cardFs.flashDevice->capacity / 0x00100000U) * 15000U) == true)
                {
                    if ((cardFs.flashDevice->features & SERIALFLASH_FEATURE_EPE) != 0U)
                    {
                        /* check EPE to make sure everything is ok */
                        if((serialflash_read_status(spi, channel) & SERIALFLASH_STATUS_EPE) == 0U)
                        {
                            retVal = true;                    
                        }
                    }
                    else
                    {
                        retVal = true;
                    }                 
                }
            }
            else
            {
                retVal = true;
            }
        }
    }
    
    return (retVal);
}

/*!
 *
 * @brief        Blank Checks a block
 *        
 * @param[in]    addr    address inside of block
 *
 * @return       bool    true=blank
 *
 * @note         blank is all bytes in block are 0xFF
 *
 */
bool serialflash_blank_check_block (cardFilesystem_t cardFs, cardSpi_t spi, cardSpi_channels_e channel, uint32_t addr)
{
    #define  BYTES_TO_BLANK_CHECK   16U
    
    uint32_t bytesread;
    bool retVal = true;
    uint32_t buff[BYTES_TO_BLANK_CHECK / sizeof(uint32_t)];         /* 16 bytes but 4 byte alligned */
    
    addr &= ~(SERIALFLASH_MIN_BLOCK_SIZE - 1U);         /* first address of block */
    
    for (bytesread = 0U; ((bytesread < SERIALFLASH_MIN_BLOCK_SIZE) && (retVal == true)); bytesread += BYTES_TO_BLANK_CHECK)
    {
        if (serialflash_read_stream(cardFs, spi, channel, addr, BYTES_TO_BLANK_CHECK, (uint8_t*)buff) == false)
        {
            retVal = false;
            break;
        }
        else
        {
            for (uint8_t idx = 0U; idx < sizeof(uint32_t); idx++)
            {
                if (buff[idx] != 0xFFFFFFFFU)
                {
                    retVal = false;
                    break;
                }
            }
            addr += BYTES_TO_BLANK_CHECK;
        }
    }
    
    return (retVal);
}


/*************************** PRIVATE FUNCTIONS **************************/

/*!
 *
 * @brief        Reads the manufactures vendor and product ID 
 *        
 * @param[in]    None
 *
 * @return       uint32_t    (manufacture id bits 16-23)
 *                           (product id bits 0-15)
 *
 * @note         Reading the device ID can be done at any state 
 *               returns 0 if nothing 
 *
 */ 
static uint32_t serialflash_read_id (cardSpi_t spi, cardSpi_channels_e channel)
{
    uint32_t retVal = 0U;
    uint8_t rxBuff[3];
    uint8_t txBuff[1];

    txBuff[0] = SERIALFLASH_OPCODE_READ_ID;
    cardSpi_selectDevice(spi, channel);
    HAL_SPI_Transmit(&spi.spi_handle, txBuff, 1U, 100U);
    HAL_SPI_Receive(&spi.spi_handle, rxBuff, 3U, 100U);
    cardSpi_deselectDevice(spi);
        
    retVal = rxBuff[0];
    retVal <<= 8;
    retVal |=rxBuff[1];
    retVal <<= 8;
    retVal |= rxBuff[2];
  
    return (retVal);
}
/*!
 *
 * @brief        Reads the status register
 *        
 * @param[in]    None
 *
 * @return       uint8_t content of status register
 *
 * @note         
 *
 */
static uint8_t serialflash_read_status (cardSpi_t spi, cardSpi_channels_e channel)
{
    uint8_t txBuff[2];
    uint8_t rxBuff[2];

    txBuff[0] = SERIALFLASH_OPCODE_READ_STATUS;
    txBuff[1] = 0x00;
    cardSpi_selectDevice(spi, channel);
    HAL_SPI_TransmitReceive(&spi.spi_handle, txBuff, rxBuff, 2U, 100U);
    cardSpi_deselectDevice(spi);
    
    return (rxBuff[1]);
}

/*!
 *
 * @brief        Wait for device to go to not busy (ready)
 *        
 * @param[in]    maxtime time to wait in ms
 *
 * @return       true=device ready in time. false=device not ready 
 *
 * @note         
 *
 */ 
static bool serialflash_wait_busy (cardSpi_t spi, cardSpi_channels_e channel, uint32_t maxtime)
{
    uint32_t targtick;
    bool retVal = false;
  
    if (maxtime == 0U)
    {
        maxtime = 1U;
    }
    
    targtick = maxtime + osKernelGetTickCount();
    
    while (targtick > osKernelGetTickCount())
    {
        if((serialflash_read_status(spi, channel) & SERIALFLASH_STATUS_BSY) == 0U)
        {
            retVal = true;
            break;
        }
    }
  
    return (retVal);
}

#if 0
/*****************************************************************************//*!
*
* @brief        Unprotects all sectors so they can be written 
*        
* @param[in]    None
*
* @return       None
*
* @note         
*
*****************************************************************************/
bool serialflash_UnprotectDevice(void)
{
  bool retVal = false;
  
  /* basic sanity checks first */
  if( (serialflash_Device != SERIALFLASH_NONE)&&
     (serialflash_Device != SERIALFLASH_FAIL))
  { 
    /* wait for ready */
    if(serialflash_wait_busy(SERIALFLASH_PAGE_WRITE_TIME) == true)
    {
      /* now set write latch */
      serialflash_spiTxfr->noOfTxBytes = 1U;
      serialflash_spiTxfr->noOfRxBytes = 0U;
      serialflash_spiTxfr->TxBuff[0] = SERIALFLASH_OPCODE_WRITE_ENABLE;
      spi_transfer(serialflash_spiTxfr);
      
      
      /* now unprotect all sectors */
      serialflash_spiTxfr->noOfTxBytes = 2U;
      serialflash_spiTxfr->noOfRxBytes = 0U;
      serialflash_spiTxfr->TxBuff[0] = SERIALFLASH_OPCODE_WRITE_STATUS;
      serialflash_spiTxfr->TxBuff[1] = 0U;               /* see section 9.2 of data sheet */
      spi_transfer(serialflash_spiTxfr);
      
      /* wait for the write then check the unprotected status */
      if(serialflash_wait_busy(SERIALFLASH_PAGE_WRITE_TIME) == true)
      {
        retVal = serialflash_CheckUnProtected();
      }
      
    }
  }
  return retVal;
}
/*****************************************************************************//*!
*
* @brief        Protects all sectors so they can't be written 
*        
* @param[in]    None
*
* @return       None
*
* @note         
*
*****************************************************************************/
bool serialflash_ProtectDevice(void)
{
  bool retVal=false;
  
  /* basic sanity checks first */
  if( (serialflash_Device != SERIALFLASH_NONE)&&
     (serialflash_Device != SERIALFLASH_FAIL))
  { 
    /* wait for ready */
    if(serialflash_wait_busy(SERIALFLASH_PAGE_WRITE_TIME) == true)
    {
      /* now set write latch */
      serialflash_spiTxfr->noOfTxBytes = 1U;
      serialflash_spiTxfr->noOfRxBytes = 0U;
      serialflash_spiTxfr->TxBuff[0] = SERIALFLASH_OPCODE_WRITE_ENABLE;
      spi_transfer(serialflash_spiTxfr);
      
      
      /* now unprotect all sectors */
      serialflash_spiTxfr->noOfTxBytes = 2U;
      serialflash_spiTxfr->noOfRxBytes = 0U;
      serialflash_spiTxfr->TxBuff[0] = SERIALFLASH_OPCODE_WRITE_STATUS;
      serialflash_spiTxfr->TxBuff[1] = 0x7FU;                /* see section 9.2 of data sheet */
      spi_transfer(serialflash_spiTxfr);
      retVal = true;
    }
  }
  return retVal;
}
/*****************************************************************************//*!
*
* @brief        gets the size in bytes of the attached device  
*        
* @param[in]    None
*
* @return       uint32_t size in bytes
*
* @note         
*
*****************************************************************************/
uint32_t serialflash_GetSize(void)
{
  return serialflash_DeviceSize;
}
/*****************************************************************************//*!
*
* @brief        get the manufactures name and device 
*        
* @param[in]    pbuff pointer where to place the string
*
* @return       length of string
*
* @note         <MANU>-<DEVICE> or "None Fitted"
*
*****************************************************************************/
uint8_t serialflash_GetName(uint8_t *pbuff)
{
  
  const serialflash_CHIPID_T *chipid;
  
  if(pbuff != NULL)
  {
    if((serialflash_Device != SERIALFLASH_NONE) && (serialflash_Device != SERIALFLASH_FAIL))
    {
      chipid=&serialflash_ChipID[serialflash_Device - 1U];
      strcpy((char*)pbuff, (char const*)chipid->MaunString);
      strcat((char*)pbuff, "-");
      strcat((char*)pbuff, (char const*)chipid->ProdString);
      
    }
    else
    {
      strcpy((char*)pbuff, "None Fitted");
    }
  }
  
  
  return (uint8_t)strlen((char*)pbuff);
  
}



/****************************** GROUP END *******************************/
//@} // BSPGroup
#endif
