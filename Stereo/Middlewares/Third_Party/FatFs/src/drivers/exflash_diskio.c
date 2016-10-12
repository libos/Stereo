/**
  ******************************************************************************
  * @file    flash_diskio.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    22-April-2014
  * @brief   Flash Disk I/O driver
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "exflash_diskio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS EXFLASH_initialize (void);
DSTATUS EXFLASH_status (void);
DRESULT EXFLASH_read (BYTE*, DWORD, BYTE);

#if _USE_WRITE == 1
  DRESULT EXFLASH_write (const BYTE*, DWORD, BYTE);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT EXFLASH_ioctl (BYTE, void*);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  ExFlash_Driver =
{
  EXFLASH_initialize,
  EXFLASH_status,
  EXFLASH_read, 
#if  _USE_WRITE
  EXFLASH_write,
#endif  /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  EXFLASH_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS EXFLASH_initialize(void)
{
  Stat = STA_NOINIT;
  
  /* Configure the SDRAM device */
  EXFLASH_INIT();
  
  Stat &= ~STA_NOINIT;
  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS EXFLASH_status(void)
{
  Stat = STA_NOINIT;

//  if(BSP_EXFLASH_GetStatus() == EXFLASH_OK)
  {
    Stat &= ~STA_NOINIT;
  }
	//Stat &= ~STA_NOINIT;
  
  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT EXFLASH_read(BYTE *buff, DWORD sector, BYTE count)
{
  DRESULT res = RES_OK;
//  SPI_Flash_Read(uint32_t Dst, uint32_t NByte,uint8_t* RcvBufPt);
  if(SPI_Flash_Read((uint32_t) (sector * W25X16_BLOCK_SIZE),count*W25X16_BLOCK_SIZE,buff)!= EXFLASH_OK)
  {
    res = RES_ERROR;
  }
  return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT EXFLASH_write(const BYTE *buff, DWORD sector, BYTE count)
{
  DRESULT res = RES_OK;
  //uint8_t SPI_Flash_Write(uint32_t Dst,uint32_t NByte,uint8_t* SndbufPt)
	//SPI_Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
	//if(SPI_Flash_Write((uint32_t)(sector * W25X16_BLOCK_SIZE),count*W25X16_BLOCK_SIZE,(uint8_t *)buff) != EXFLASH_OK)
	if(SPI_Flash_Write((uint32_t)(sector * W25X16_BLOCK_SIZE),count*W25X16_BLOCK_SIZE,(uint8_t *)buff) != EXFLASH_OK)
  {
    res = RES_ERROR;
  }
  
  return res;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */

#if _USE_IOCTL == 1
DRESULT EXFLASH_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT:
    *(DWORD*)buff = W25X16_BLOCK_NUM;
    res = RES_OK;
    break;
  
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = W25X16_BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = W25X16_BLOCK_SIZE;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */
  
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

