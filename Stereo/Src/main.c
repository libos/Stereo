/**
	******************************************************************************
	* File Name					: main.c
	* Date							 : 30/10/2014 20:02:37
	* Description				: Main program body
	******************************************************************************
	*
	* COPYRIGHT(c) 2014 STMicroelectronics
	*
	* Redistribution and use in source and binary forms, with or without modification,
	* are permitted provided that the following conditions are met:
	*	 1. Redistributions of source code must retain the above copyright notice,
	*			this list of conditions and the following disclaimer.
	*	 2. Redistributions in binary form must reproduce the above copyright notice,
	*			this list of conditions and the following disclaimer in the documentation
	*			and/or other materials provided with the distribution.
	*	 3. Neither the name of STMicroelectronics nor the names of its contributors
	*			may be used to endorse or promote products derived from this software
	*			without specific prior written permission.
	*
	* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*
	******************************************************************************
	*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sdram_diskio.h" /* defines SDRAMDISK_Driver as external */
#include "sd_diskio.h" /* defines SD_Driver as external */
#include "exflash_diskio.h" /* defines Exflash_Driver as external */
#include "bsp_driver_cam.h"
#include "usb_device.h"
#include "dataflash.h"
#include "misc.h"


#define MPUAddr 0x68

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

DAC_HandleTypeDef hdac;

DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

I2C_HandleTypeDef hi2c2;

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

SPI_HandleTypeDef hspi5;

UART_HandleTypeDef huart4;

SDRAM_HandleTypeDef hsdram1;
static FMC_SDRAM_CommandTypeDef sdramCommand;

uint8_t SDRAMDISK_DriverNum;			/* FatFS SDRAMDISK part */
char SDRAMDISK_Path[4];					 /* SDRAMDISK logical drive path */
uint8_t SD_DriverNum;			/* FatFS SD part */
char SD_Path[4];					 /* SD logical drive path */
uint8_t ExFlash_DriverNum;			/* FatFS ExFlash part */
char ExFlash_Path[4];					 /* ExFlash logical drive path */
extern Diskio_drvTypeDef	ExFlash_Driver;

static FATFS fatfs;
//static FATFS sdramfs;
/* USER CODE BEGIN 0 */

	/*	目前公认的标准是，存储体中电容的数据有效保存期上限是64ms（毫秒，1/1000秒），
	 *	也就是说每一行刷新的循环周期是64ms。这样刷新速度就是：行数量/64ms。我们在看内
	 *	存规格时，经常会看到4096 Refresh Cycles/64ms或8192 RefreshCycles/64ms的标识，
	 *	这里的4096与8192就代表这个芯片中每个L-Bank的行数。刷新命令一次对一行有效，
	 *	发送间隔也是随总行数而变化，4096行时为15.625μs（微秒，1/1000毫秒），
	 *	8192行时就为7.8125μs。使用 (15.62 us x Freq) - 20 求得
	 */
#define REFRESH_COUNT			 ((uint32_t)0x027C)	 /* SDRAM refresh counter 8192 (84Mhz SD clock) */
//#define REFRESH_COUNT			 ((uint32_t)0x050C)	 /* SDRAM refresh counter 4096 (84Mhz SD clock) */

#define BSP_LED1_OFF() 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)
#define BSP_LED2_OFF() 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET)
#define BSP_LED1_ON() 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)
#define BSP_LED2_ON() 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET)
#define BSP_LED1_TOGGLE() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)
#define BSP_LED2_TOGGLE() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET)
/* USER CODE END 0 */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_DAC_Init(void);
static void MX_DMA_Init(void);
static void MX_DCMI_Init(void);
static void MX_FMC_Init(void);
static void MX_I2C2_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI5_Init(void);
static void MX_UART4_Init(void);
static void SDRAM_Initialization_Sequence(FMC_SDRAM_CommandTypeDef *Command);
void Debug(char *msg);
void uartWrite(void *msg);

void i2cWrite(uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t txlen);
void i2cRead(uint8_t I2C_ADDRESS, uint8_t RegAddr, uint8_t *aRxBuffer, uint8_t rxlen);

__IO static uint8_t flag_frame_end;
static uint32_t DST_Buffer[0xFF];




#define CHANGE_REG_NUM 162
const uint16_t change[CHANGE_REG_NUM][2]=
{	
  
	{ 0xf0,0x0000 }, //Sensor address page 0  
	{ 0x0d,0x0009 },   
	{ 0x0d,0x0029 },   
	{ 0x0d,0x0008 },

	{0,10 }, //
	{0x9d,0x3ca0 }, //

	{ 0xf0,0x0001 }, //Sensor address page 1 
	{ 0x9d,0x3ca0 },   
	{ 0xf0,0x0002 }, //Sensor address page 2  
	{ 0x24,0x5f20 },   
	{ 0x28,0xef02 },   
	{ 0x5f,0x4143 },   
	{ 0x02,0x00ee },   
	{ 0x03,0x291a },   
	{ 0x04,0x02a4 },   //0x2a4
	{ 0x09,0x0068 },   
	{ 0x0a,0x002a },   
	{ 0x0b,0x0004 },   
	{ 0x0c,0x0093 },   
	{ 0x0d,0x0082 },   
	{ 0x0e,0x0040 },   
	{ 0x0f,0x005f },   
	{ 0x10,0x004e },   
	{ 0x11,0x005b },   
	{ 0x15,0x00c9 },   
	{ 0x16,0x005e },   
	{ 0x17,0x009d },   
	{ 0x18,0x0006 },   
	{ 0x19,0x0089 },   
	{ 0x1a,0x0012 },   
	{ 0x1b,0x00a1 },   
	{ 0x1c,0x00e4 },   
	{ 0x1d,0x007a },   
	{ 0x1e,0x0064 },   
	{ 0xf6,0x005f },   
	{ 0xf0,0x0001 },   
	{ 0x81,0x0000 },   
	{ 0x82,0x0000 },   
	{ 0x83,0x0000 },   
	{ 0x84,0x0000 },   
	{ 0x85,0x0000 },   
	{ 0x86,0x0000 },   
	{ 0x87,0x0000 },   
	{ 0x88,0x0000 },   
	{ 0x89,0x0000 },   
	{ 0x8a,0x0000 },   
	{ 0x8b,0x0000 },   
	{ 0x8c,0x0000 },   
	{ 0x8d,0x0000 },   
	{ 0x8e,0x0000 },   
	{ 0x8f,0x0000 },   
	{ 0x90,0x0000 },   
	{ 0x91,0x0000 },   
	{ 0x92,0x0000 },   
	{ 0x93,0x0000 },   
	{ 0x94,0x0000 },   
	{ 0x95,0x0000 },   
	{ 0xb6,0x0000 },   
	{ 0xb7,0x0000 },   
	{ 0xb8,0x0000 },   
	{ 0xb9,0x0000 },   
	{ 0xba,0x0000 },   
	{ 0xbb,0x0000 },   
	{ 0xbc,0x0000 },   
	{ 0xbd,0x0000 },   
	{ 0xbe,0x0000 },   
	{ 0xbf,0x0000 },   
	{ 0xc0,0x0000 },   
	{ 0xc1,0x0000 },   
	{ 0xc2,0x0000 },   
	{ 0xc3,0x0000 },   
	{ 0xc4,0x0000 },   
  //0xffff,0x0006, 
	{ 0xf0,0x0001 },  //Sensor address page 1
  { 0x3a , 0x0102 },//RG565
  { 0x9b , 0x0102 },
        
	{ 0x53,0x1c12 },   
	{ 0x54,0x402a },   
	{ 0x55,0x7c62 },   
	{ 0x56,0xa994 },   
	{ 0x57,0xcfbc },   
	{ 0x58,0xe000 },   
	{ 0xdc,0x1c12 },   
	{ 0xdd,0x402a },   
	{ 0xde,0x7c62 },   
	{ 0xdf,0xa994 },   
	{ 0xe0,0xcfbc },   
	{ 0xe1,0xe000 },   
	{ 0x34,0x0010 },   
	{ 0x35,0xf010 },   
	{ 0xf0,0x0000 },  //Sensor address page 0 
//  { 0x03 , 480+400 },//240+400
//	{ 0x04 , 814+400 },//320+400
//  { 0x01 , 12+400 },
//  { 0x02 , 28+400 },
	{ 0x05,0x00f1 },   
	{ 0x06,0x000d },   
	{ 0x07,0x00d9 },   
	{ 0x08,0x000d },   
	{ 0x20,0x0100 },   
	{ 0x21,0x0400 },   
	{ 0x22,0x0d2b },   
	{ 0x24,0x8000 },   
	{ 0x59,0x0018 },   
	//{ 0xffff,0x0003, 
  { 0x33 , 0x0400 },
  { 0x34 , 0xc019 }, 
  { 0x3f , 0x2020 },
  { 0x40 , 0x2020 },      
  { 0x5a , 0xc00a },
  { 0x70 , 0x7b0a },
  { 0x71 , 0xff00 },
  { 0x72 , 0x190e },
  { 0x73 , 0x180f },
  { 0x74 , 0x5732 },
  { 0x75 , 0x5634 },
  { 0x76 , 0x7335 },
  { 0x77 , 0x3012 },
        { 0x78 , 0x7902 },
        { 0x79 , 0x7506 },
        { 0x7a , 0x770a },
        { 0x7b , 0x7809 },
        { 0x7c , 0x7d06 },
        { 0x7d , 0x3110 },
        { 0x7e , 0x007e },
        { 0x80 , 0x5904 },
        { 0x81 , 0x5904 },
        { 0x82 , 0x570a },
        { 0x83 , 0x580b },
        { 0x84 , 0x470c },
        { 0x85 , 0x480e },
        { 0x86 , 0x5b02 },
        { 0x87 , 0x005c },
	{ 0xf0,0x0002 }, //Sensor address page 2   
	{ 0x39,0x06c2 },   
	{ 0x3a,0x05f9 },   
	{ 0x3b,0x0410 },   
	{ 0x3c,0x049d },   
	{ 0x57,0x0104 },   
	{ 0x58,0x0138 },   
	{ 0x59,0x0126 },   
	{ 0x5a,0x0161 },   
	{ 0x5c,0x120d }, 
  
	{ 0x9b,0x0300 },  // rgb//
	{ 0x3a,0x0300 }, //
	{ 0xf0,0x0001 },   //Sensor address page 1
        
        { 159, 0  },
        { 160, 1280},  // 
        { 161, 700},//   
        { 162, 0  },    
        { 163, 960}, //    
        { 164, 480},  
        { 165, 0   },
        
        { 166, 1280},//
        { 167, 700},//
        { 168, 0   },
        { 169, 960}, //
        { 170, 480},
	//{ 0xffff,0x0003, 
	{ 0xf0,0x0002 },//Sensor address page 2 
	{ 0xd2,0x007f },   
	{ 0x5b,0x0000 },   
	{ 0xcc,0x0004 },   
	{ 0xcb,0x0001 },   
	{ 0xf0,0x0000 },//Sensor address page 0

};
/* Start ----------------------------------*/
int main(void)
{

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* System interrupt init*/
	/* Sets the priority grouping field */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	CAM1_ENABLE();
	CMA2_DISABLE();
	
	CAM1_OFF();
	CAM2_OFF();
	HAL_Delay(100);
	CAM1_ON();
	MX_UART4_Init();
	
	MX_DMA_Init();
	//MX_ADC1_Init();
	//MX_ADC2_Init();
	//MX_DAC_Init();
	
	MX_DCMI_Init();

	MX_SDIO_SD_Init();
	MX_SPI5_Init();

	MX_USB_DEVICE_Init();
	
	MX_FMC_Init();
 	MX_I2C2_Init();

	
/*## FatFS: Link the SDRAMDISK disk I/O driver ###############################*/
	//ExFlash_DriverNum = FATFS_LinkDriver(&ExFlash_Driver, ExFlash_Path);
	/*## FatFS: Link the SD disk I/O driver ###############################*/
	SD_DriverNum = FATFS_LinkDriver(&SD_Driver, SD_Path);
	
#if 0
	FRESULT res = f_mount(&fatfs,SD_Path,1);
	
	if (res == FR_OK) {
		FRESULT res2 = f_chdrive(SD_Path);
		if(res2 == FR_OK)	
		{
			Debug("Change Drive Done!");
			writeTFCard(&fatfs,SD_Path);
		}
	}else{
		char errmsg[30];
		sprintf(errmsg,"Fail to mount SD. Error Code is %d",res);
		Debug(errmsg);
	}
#endif
	
#if 0
	writeTFCard(SD_Path);
#endif
#if 0	//Done
	testExFlash();
	writeExFlash();
#endif
#if 0
	SDRam16bTest();
#endif
#if 0
	SDRam8bTest();
#endif
	
	Debug("UART");
	
	//HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);	//LED OFF

	
	/**************Start Cam*********************/
	// Enable MAX3372E (disable three_state pin)
	//MAX3372_ENABLE();
	


#if 1

	
	//Check Chip Version
	uint8_t mt_data[2] = {0x00,0x00};
	char mtmsg[20];
	uint8_t reg_data[3];
//	i2cRead(MT2Addr,0xFF,mt_data,2);

//	sprintf(mtmsg,"mt1 chip version is %02x%02x",mt_data[0],mt_data[1]);
//	
//	Debug(mtmsg);
//	reg_data[0] = 0xF0;
//	reg_data[1] = 0;
// 	reg_data[2] = 0;
//	i2cWrite(MT2Addr,reg_data,3);
//	reg_data[0] = 0x0D;
//	//00010100		Disable
//	reg_data[1] = 1;
// 	reg_data[2] = 4;
//	i2cWrite(MT2Addr,reg_data,3);
//	
//	i2cRead(MT2Addr,0xFF,mt_data,2);
//	sprintf(mtmsg,"mt1 chip version is %02x%02x",mt_data[0],mt_data[1]);
//	Debug(mtmsg);

	i2cRead(MT1ADDR,0xFF,mt_data,2);
	sprintf(mtmsg,"mt2 chip version is %02x%02x",mt_data[0],mt_data[1]);
	Debug(mtmsg);
//************
	for(int i=0;i<CHANGE_REG_NUM;i++)
	{
		reg_data[0] = change[i][0];
		
		reg_data[1] = change[i][1]  >> 8;
		reg_data[2] = change[i][1] & 0x00ff;
		//i2cWrite(MT1ADDR,reg_data,3);
	}
	Debug("Config Done");

	uint32_t addr = SDRAM_DEVICE_ADDR ;
	uint8_t reso = RESO_VGA;
	
	//HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)&DST_Buffer,0xFF);//GetSize(reso));//0xFF);//9600);//
	flag_frame_end = SET;
	HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)addr,0x1006);

	while(flag_frame_end != RESET)
	{
	}

	//HAL_DCMI_Stop(&hdcmi);
  char partx[30];
  for(int i = 0 ;i< 0xF;i++)
	{
//		if(DST_Buffer[i] == 0)
//		{
//			Debug("Empty");
//			continue;
//		}
		memset(partx,0,30);
		//sprintf(partx,"data[%03d]:0x%08x",i,DST_Buffer[i]);
		sprintf(partx,"0x%08x",*(__IO uint32_t *) (SDRAM_DEVICE_ADDR+i*4));
		//sprintf(partx,"%08x",DST_Buffer[i]);uartWrite(partx);
		Debug(partx);
	}	
	
#endif

	// Setting C1
	// Read Camera	DCMI file1
	 // Enable SDCard
	// Write to SDCard						// Or write to SDRAM first then write to sdcard
	// When Finished
		// File process
		// Disable C1
		// LED1 ON

	// Disable C1
	// Enable C2	
	// Setting C2
	// Read Camera C2
	// Write to SDCard						// Or write to SDRAM first then write to sdcard
	// When Finished
		// Disable C2
		// LED2 ON

	// Test Buzz
	/* Infinite loop */
	while (1)
	{
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_2);
		HAL_Delay(1000);
	}

}
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	Debug("Frame End");
	flag_frame_end = RESET;
}
void HAL_DCMI_DMA_CompleteCallback(DCMI_HandleTypeDef *hdcmi)
{
	Debug("DMA Complete");
	//flag_frame_end = RESET;
}
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
	char msg[30];
	sprintf(msg,"DCMI ERROR CODE:%x",hdcmi->ErrorCode);
	Debug(msg);
}

void i2cWrite(uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t txlen) 
{	
		/* -> Start the transmission process */
		/* While the I2C in reception process, user can transmit data through "aTxBuffer" buffer */
		while(HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)I2C_ADDRESS<<1, (uint8_t*)aTxBuffer, (uint16_t)txlen, (uint32_t)DEFTIMEOUT)!= HAL_OK)
		{
				if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
				{
						Debug("EW");
						break;
				}
		}

		/* -> Wait for the end of the transfer */
		/* Before starting a new communication transfer, you need to check the current
		 * state of the peripheral; if it’s busy you need to wait for the end of current
		 * transfer before starting a new one.
		 * For simplicity reasons, this example is just waiting till the end of the
		 * transfer, but application may perform other tasks while transfer operation
		 * is ongoing.
		 */
			while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
			{
			}
}
void i2cRead(uint8_t I2C_ADDRESS, uint8_t RegAddr, uint8_t *aRxBuffer, uint8_t rxlen)
{
		/* -> Lets ask for register's address */
		i2cWrite(I2C_ADDRESS, &RegAddr, 1);

		/* -> Put I2C peripheral in reception process */
		while(HAL_I2C_Master_Receive(&hi2c2, (uint16_t)I2C_ADDRESS<<1, aRxBuffer, (uint16_t)rxlen, (uint32_t)DEFTIMEOUT) != HAL_OK)
		{
				/* Error_Handler() function is called when Timeout error occurs.
				 * When Acknowledge failure occurs (Slave don't acknowledge it's address)
				 * Master restarts communication
				 */
				if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
				{
						Debug("ER");
						break;
				}
		}

		/* -> Wait for the end of the transfer */
		/* Before starting a new communication transfer, you need to check the current
		 * state of the peripheral; if it’s busy you need to wait for the end of current
		 * transfer before starting a new one.
		 * For simplicity reasons, this example is just waiting till the end of the
		 * transfer, but application may perform other tasks while transfer operation
		 * is ongoing.
		 **/
		while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
		{
		}
}
void Debug(char *msg)
{
	uartWrite("Debug: ");
	uartWrite(msg);
	uartWrite("\r\n");
}
void uartWrite(void *data)
{
	HAL_UART_Transmit(&huart4,(uint8_t *)data,(uint16_t)strlen(data),(uint32_t)DEFTIMEOUT);
}
/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __DMA2_CLK_ENABLE();

  /* DMA interrupt init */
	
  /* Sets the priority grouping field */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}

/* DCMI init function */
void MX_DCMI_Init(void)
{

	hdcmi.Instance = DCMI;
	hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
	hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;
	hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
	hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
	hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
	hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	hdcmi.Init.JPEGMode = DCMI_JPEG_ENABLE;//DCMI_JPEG_DISABLE;
	HAL_DCMI_Init(&hdcmi);

}

/**
	* @brief	Perform the SDRAM exernal memory inialization sequence
	* @param	hsdram: SDRAM handle
	* @param	Command: Pointer to SDRAM command structure
	* @retval None
	*/
static void SDRAM_Initialization_Sequence(FMC_SDRAM_CommandTypeDef *Command)
{
	__IO uint32_t tmpmrd =0;
	/* Step 3:	Configure a clock configuration enable command */
	Command->CommandMode 			 = FMC_SDRAM_CMD_CLK_ENABLE;
	Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber 	 = 1;
	Command->ModeRegisterDefinition = 0;

	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram1, Command, 0x1000);

	/* Step 4: Insert 100 ms delay */
	HAL_Delay(100);
		
	/* Step 5: Configure a PALL (precharge all) command */ 
	Command->CommandMode 			 = FMC_SDRAM_CMD_PALL;
	Command->CommandTarget 			 = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber 	 = 1;
	Command->ModeRegisterDefinition = 0;

	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram1, Command, 0x1000);	
	
	/* Step 6 : Configure a Auto-Refresh command */ 
	Command->CommandMode 			 = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber 	 = 4;
	Command->ModeRegisterDefinition = 0;

	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram1, Command, 0x1000);
	
	/* Step 7: Program the external memory mode register */
	tmpmrd = (uint32_t)	SDRAM_MODEREG_BURST_LENGTH_2				 |
										 SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL	 |
										 SDRAM_MODEREG_CAS_LATENCY_3					 |
										 SDRAM_MODEREG_OPERATING_MODE_STANDARD |
										 SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
										 
	Command->CommandMode 				= FMC_SDRAM_CMD_LOAD_MODE;
	Command->CommandTarget 		 	= FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber 	 = 1;
	Command->ModeRegisterDefinition = tmpmrd;

	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram1, Command, 0x1000);
	
	/* Step 8: Set the refresh rate counter */
	/* (15.62 us x Freq) - 20 */

	/* Set the device refresh counter */
	HAL_SDRAM_ProgramRefreshRate(&hsdram1, REFRESH_COUNT);//0x90);//0x90 for Freq = 10.2M//REFRESH_COUNT); 
}
/* FMC initialization function */
void MX_FMC_Init(void)
{
	FMC_SDRAM_TimingTypeDef SdramTiming;

	/** Perform the SDRAM1 memory initialization sequence
	*/
	hsdram1.Instance = FMC_SDRAM_DEVICE;
	/* hsdram1.Init */
	hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
	/* Column addressing: [8:0] */
	hsdram1.Init.ColumnBitsNumber 	= FMC_SDRAM_COLUMN_BITS_NUM_8;
	/* Row addressing: [12:0] */
	hsdram1.Init.RowBitsNumber 			= FMC_SDRAM_ROW_BITS_NUM_12;
	hsdram1.Init.MemoryDataWidth 		= FMC_SDRAM_MEM_BUS_WIDTH_16;
	hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	hsdram1.Init.CASLatency 				= FMC_SDRAM_CAS_LATENCY_3;
	hsdram1.Init.WriteProtection		= FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	hsdram1.Init.SDClockPeriod			= FMC_SDRAM_CLOCK_PERIOD_3;
	hsdram1.Init.ReadBurst					= FMC_SDRAM_RBURST_DISABLE;
	hsdram1.Init.ReadPipeDelay			= FMC_SDRAM_RPIPE_DELAY_1;
	/* SdramTiming */
	SdramTiming.LoadToActiveDelay = 16;
	SdramTiming.ExitSelfRefreshDelay = 16;
	SdramTiming.SelfRefreshTime = 16;
	SdramTiming.RowCycleDelay = 16;
	SdramTiming.WriteRecoveryTime = 16;
	SdramTiming.RPDelay = 16;
	SdramTiming.RCDDelay = 16;
	/* Timing configuration for 84 Mhz of SD clock frequency (168Mhz/2) 
	*		168MHz/2 = 84Mhz
	*/
	/* TMRD: 2 Clock cycles 1/84Mhz = 11.9ns*/
//	SdramTiming.LoadToActiveDelay		= 2;
//	/* TXSR: min=70ns (6x11.90ns) */
//	SdramTiming.ExitSelfRefreshDelay = 7;
//	/* TRAS: min=42ns (4x11.90ns) max=120k (ns) */
//	SdramTiming.SelfRefreshTime			= 4;
//	/* TRC:	min=63 (6x11.90ns) */				
//	SdramTiming.RowCycleDelay				= 7;
//	/* TWR:	2 Clock cycles */
//	SdramTiming.WriteRecoveryTime		= 4;
//	/* TRP:	15ns => 2x11.90ns */
//	SdramTiming.RPDelay							= 2;
//	/* TRCD: 15ns => 2x11.90ns */
//	SdramTiming.RCDDelay						 = 2;

	if(HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
	{
		/* Initialization Error */
			Debug("Err init fmc");
	}
	SDRAM_Initialization_Sequence(&sdramCommand);
}
/** System Clock Configuration
*/
void SystemClock_Config(void)
{

	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
															|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

}

/* ADC1 init function */
void MX_ADC1_Init(void)
{

	ADC_ChannelConfTypeDef sConfig;

		/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
		*/
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION12b;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.NbrOfDiscConversion = 1;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = EOC_SINGLE_CONV;
	HAL_ADC_Init(&hadc1);

		/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
		*/
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}

/* ADC2 init function */
void MX_ADC2_Init(void)
{

	ADC_ChannelConfTypeDef sConfig;

		/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
		*/
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	hadc2.Init.Resolution = ADC_RESOLUTION12b;
	hadc2.Init.ScanConvMode = DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.NbrOfDiscConversion = 1;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.NbrOfConversion = 1;
	hadc2.Init.DMAContinuousRequests = DISABLE;
	hadc2.Init.EOCSelection = EOC_SINGLE_CONV;
	HAL_ADC_Init(&hadc2);

		/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
		*/
	sConfig.Channel = ADC_CHANNEL_13;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);

}

/* DAC init function */
void MX_DAC_Init(void)
{

	DAC_ChannelConfTypeDef sConfig;

		/**DAC Initialization 
		*/
	hdac.Instance = DAC;
	HAL_DAC_Init(&hdac);

		/**DAC channel OUT2 config 
		*/
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2);

}


/* I2C2 init function */
void MX_I2C2_Init(void)
{

	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 300000;
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	HAL_I2C_Init(&hi2c2);

}

/* SDIO init function */
void MX_SDIO_SD_Init(void)
{

	hsd.Instance = SDIO;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = 0;
	HAL_SD_Init(&hsd, &SDCardInfo);

}

/* SPI5 init function */
void MX_SPI5_Init(void)
{

	hspi5.Instance = SPI5;
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi5.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	HAL_SPI_Init(&hspi5);	

}

/* UART4 init function */
void MX_UART4_Init(void)
{

	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart4);

}


/** Configure pins as 
				* Analog 
				* Input 
				* Output
				* EVENT_OUT
				* EXTI
				* Free pins are configured automatically as Analog (this feature is enabled through 
				* the Code Generation settings)
		 PB4	 ------> S_TIM3_CH1
*/
void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__GPIOE_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();
	__GPIOH_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();

	/*Configure GPIO pins : PE2 PE5 PE6 */
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PE3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : PC13 PC5 */
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PC14 PC15 PC6 PC7 */
	GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
		/*Configure GPIO pin : PF6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PF10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin : PC1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA2 PA3 */
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA7 PA8 PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PC4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB12 PB13 
													 PB14 PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_13 
													|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PB2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PD11 PD12 PD13 */
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : PG2 PG3 PG6 PG7 
													 PG9 PG12 PG14 */
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7 
													|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pins : PD4 PD5 PD6 PD7 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : PG13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : PB4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}


#ifdef USE_FULL_ASSERT

/**
	 * @brief Reports the name of the source file and the source line number
	 * where the assert_param error has occurred.
	 * @param file: pointer to the source file name
	 * @param line: assert_param error line source number
	 * @retval None
	 */
void assert_failed(uint8_t* file, uint32_t line)
{
	char msg[50];
	sprintf(msg,"Wrong parameters value: file %s on line %d\r\n", file, line);
	Debug(msg);

}

#endif

/**
	* @}
	*/ 

/**
	* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
