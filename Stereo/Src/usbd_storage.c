/**
  ******************************************************************************
  * @file           : usbd_Storage_if.c
  * @author         : MCD Application Team
  * @version        : V1.1.0
  * @date           : 19-March-2012
  * @brief          : Memory management layer
  ******************************************************************************
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage.h"

extern void Debug(char *msg);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* USB handler declaration */
/* Handle for USB Full Speed IP */
USBD_HandleTypeDef  *hUsbDevice_0;

extern USBD_HandleTypeDef hUsbDeviceFS;


/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define LUN_HAS_SDRAM 0
#define ONLY_SDCARD 0
#define LUN_HAS_EXFLASH 1

#if LUN_HAS_SDRAM
	#define STORAGE_LUN_NBR                 2
#endif

#if LUN_HAS_EXFLASH
	#define STORAGE_LUN_NBR                 2  
#endif

#if ONLY_SDCARD
	#define STORAGE_LUN_NBR                 1
#endif

//#define Storage_BLK_NBR                 BSP_SD_getBlockSize() 
////0x10000  
//#define Storage_BLK_SIZ                 BSP_SD_getCapacity() 
////0x200

static int8_t Storage_Init_FS (uint8_t lun);
static int8_t Storage_GetCapacity_FS (uint8_t lun, 
                           uint32_t *block_num, 
                           uint16_t *block_size);
static int8_t  Storage_IsReady_FS (uint8_t lun);
static int8_t  Storage_IsWriteProtected_FS (uint8_t lun);
static int8_t Storage_Read_FS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);
static int8_t Storage_Write_FS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);
static int8_t Storage_GetMaxLun_FS (void);


static uint8_t Storage_started = 0;
/* USER CODE BEGIN 0 */ 
/* USB Mass SDCard Standard Inquiry Data */
int8_t  Storage_Inquirydata_FS[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		//0x00 not removable 0x80 removable
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN -5),
  0x00,
  0x00,	
  0x00,
  'M', 'a', 'x', 't', 'a', 'i', 'n', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  'S', 'D', 'C', 'a', 'r', 'd', ' ', ' ',
  '1', '.', '0' ,'5',                     /* Version      : 4 Bytes */
#if LUN_HAS_EXFLASH
 /* LUN 1 */
  0x00,		
  0x80,		//0x00 not removable 0x80 removable
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN -5),
  0x00,
  0x00,	
  0x00,
  'M', 'a', 'x', 't', 'a', 'i', 'n', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
  '1', '.', '0' ,'5',                     /* Version      : 4 Bytes */
#endif
#if LUN_HAS_SDRAM
  /* LUN 1 */
  0x00,		
  0x80,		//0x00 not removable 0x80 removable
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN -5),
  0x00,
  0x00,	
  0x00,
  'M', 'a', 'x', 't', 'a', 'i', 'n', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  'S', 'D', 'R', 'A', 'M', ' ', ' ', ' ',
  '1', '.', '0' ,'0',                     /* Version      : 4 Bytes */
#endif
}; 
/* USER CODE END 0 */ 

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  Storage_Init_FS,
  Storage_GetCapacity_FS,
  Storage_IsReady_FS,
  Storage_IsWriteProtected_FS,
  Storage_Read_FS,
  Storage_Write_FS,
  Storage_GetMaxLun_FS,
  Storage_Inquirydata_FS,
};

/*******************************************************************************
* Function Name  : Storage_Init_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t Storage_Init_FS (uint8_t lun)
{
//	if (!BSP_SD_IsDetected()) {
//		return -1;
//	}
	Storage_started = 1;
	return USBD_OK;

}

/*******************************************************************************
* Function Name  : Storage_GetCapacity_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t Storage_GetCapacity_FS (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
	if(lun == 0)
	{
		*block_size = BSP_SD_getBlockSize();
		*block_num  = BSP_SD_getCapacity()/(*block_size);
	}
#if LUN_HAS_EXFLASH
	else if(lun == 1)
	{
		*block_size = W25X16_BLOCK_SIZE;
		*block_num  = W25X16_BLOCK_NUM;
	}
#endif
#if LUN_HAS_SDRAM
	else if(lun == 1){
		*block_size = 512;
		*block_num  = 64;///(*block_size);
	}
#endif	
#if 0
	char msg[40];
	sprintf(msg,"BlockSize is %d\t\tBlockNum is %d",*block_size,*block_num);
	Debug(msg);
#endif	
  return USBD_OK;
}

/*******************************************************************************
* Function Name  : Storage_IsReady_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  Storage_IsReady_FS (uint8_t lun)
{
	 if (Storage_started) {
					return USBD_OK;
	 }
	 return -1;
}

/*******************************************************************************
* Function Name  : Storage_IsWriteProtected_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  Storage_IsWriteProtected_FS (uint8_t lun)
{
  return (USBD_OK);
}

/*******************************************************************************
* Function Name  : Storage_Read_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t Storage_Read_FS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,                       
                        uint16_t blk_len)
{
	if(lun == 0){
		if (SD_read(buf,blk_addr,blk_len) != 0) {	
			Debug("SDCard Read ERR");
			return -1;
		}
		//Debug("SDCard Read OK");
	}
	
#if LUN_HAS_EXFLASH
	else if(lun == 1){
		if (EXFLASH_read(buf,blk_addr,blk_len) != 0) {	
			Debug("ExFlash Read ERR");
			return -1;
		}
		//Debug("ExFlash Read OK");
	}
#endif
	
#if LUN_HAS_SDRAM
	else if(lun == 1){
		if (SDRAMDISK_read(buf,blk_addr,blk_len) != 0) {	
			Debug("SDRAM Read ERR");
			return -1;
		}
		//Debug("SDRAM Read OK");
	}
#endif
  return (USBD_OK);
  /* USER CODE END 6 */ 
}

/*******************************************************************************
* Function Name  : Storage_Write_FS
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t Storage_Write_FS (uint8_t lun, 
                         uint8_t *buf, 
                         uint32_t blk_addr,
                         uint16_t blk_len)
{
//	if (BSP_SD_WriteBlocks((uint32_t *)buf, blk_addr,BSP_SD_getBlockSize(), blk_len) != 0) {
	if(lun == 0){
		if (SD_write(buf, blk_addr, blk_len) != 0) {
			Debug("SDCard Write ERR");
			return -1;
		}
		//Debug("SDCard Write OK");
	}
	
#if LUN_HAS_EXFLASH
	else if(lun == 1){
		if (EXFLASH_write(buf, blk_addr, blk_len) != 0) {
			Debug("ExFlash Write ERR");
			return -1;
		}
		//Debug("ExFlash Write OK");
	}
#endif
	
#if LUN_HAS_SDRAM
	else if(lun == 1){
//		if (SDRAMDISK_write(buf,blk_addr,blk_len) != 0) {	
	//	BSP_SDRAM_ReadData(uint32_t uwStartAddress, uint32_t* pData, uint32_t uwDataSize)
		if(BSP_SDRAM_ReadData(blk_addr,(uint32_t *)buf,blk_len)){
			Debug("SDRAM Read ERR");
			return -1;
		}
		//Debug("SDRAM Write OK");
	}
#endif
  return (USBD_OK);
}

/*******************************************************************************
* Function Name  : Storage_GetMaxLun_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t Storage_GetMaxLun_FS (void)
{
  return (STORAGE_LUN_NBR-1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
