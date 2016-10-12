#include "misc.h"

DMA_HandleTypeDef     DmaHandle;
#define BUFFER_SIZE0              32
static const uint32_t aSRC_Const_Buffer[BUFFER_SIZE0]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};

static uint32_t aDST_Buffer[BUFFER_SIZE0];

/* Private function prototypes -----------------------------------------------*/
static void TransferComplete(DMA_HandleTypeDef *DmaHandle);
static void TransferError(DMA_HandleTypeDef *DmaHandle);
void DMA_TEST(void)
{
	#define DMA_STREAM               DMA2_Stream0
	#define DMA_CHANNEL              DMA_CHANNEL_0 
	#define DMA_STREAM_IRQ           DMA2_Stream0_IRQn
	#define DMA_STREAM_IRQHANDLER    DMA2_Stream0_IRQHandler
  /*##-2- Select the DMA functional Parameters ###############################*/
  DmaHandle.Init.Channel = DMA_CHANNEL;                     /* DMA_CHANNEL_0                    */                     
  DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;          /* M2M transfer mode                */           
  DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;               /* Peripheral increment mode Enable */                 
  DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable     */                   
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; /* Peripheral data alignment : Word */    
  DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    /* memory data alignment : Word     */     
  DmaHandle.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                  */  
  DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high            */  
  DmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */        
  DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;  
  DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */  
  DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */
  
  /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
  DmaHandle.Instance = DMA_STREAM;

  /*##-4- Select Callbacks functions called after Transfer complete and Transfer error */
  DmaHandle.XferCpltCallback  = TransferComplete;
  DmaHandle.XferErrorCallback = TransferError;

  /*##-5- Initialize the DMA stream ##########################################*/
  if(HAL_DMA_Init(&DmaHandle) != HAL_OK)
  {
    /* Turn LED3/LED4 on: in case of Initialization Error */
		Debug("Init Error");
		while(1)
    {
    }
  }
  
  /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
  HAL_NVIC_SetPriority(DMA_STREAM_IRQ, 0, 0);
  HAL_NVIC_EnableIRQ(DMA_STREAM_IRQ);

  /*##-7- Start the DMA transfer using the interrupt mode ####################*/
  /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
  /* Enable All the DMA interrupts */
  if(HAL_DMA_Start_IT(&DmaHandle, (uint32_t)&aSRC_Const_Buffer, (uint32_t)&aDST_Buffer, BUFFER_SIZE0) != HAL_OK)
  {
    /* Turn LED3/LED4 on: Transfer error */
		Debug("Transfer error");
		while(1)
    {
    }   
  }
}
static void TransferComplete(DMA_HandleTypeDef *DmaHandle)
{
	Debug("Transfer Complete");
}
static void TransferError(DMA_HandleTypeDef *DmaHandle)
{
		Debug("Transfer ERROR");
}

void SDRam16bTest()
{
		#define BUFFER_SIZE  15
		HAL_SDRAM_WriteProtection_Disable(&hsdram1);
		/*uint32_t writeBuffer[BUFFER_SIZE] = {0xc0c0cc01,0xd0d0dd02,0xebebeebb,0xa7a7aa73,0xb6b6bb68, \
																				 0xc8543c65,0x9562dd6c,0x58ffef6c,0xcc3cccca,0x3333332f, \
																				 0x02584464,0x95135561,0x57489e64,0x65dd9961,0xfae4562f};
		*/
		uint16_t writeBuffer16[BUFFER_SIZE] = {0xAA55,0xAA55,0xAA55,0xAA55,0xAA55, \
																				 0xAA55,0xAA55,0xAA55,0xAA55,0xAA55, \
																				 0xAA55,0xAA55,0xAA55,0xAA55,0xAA55};
		for(int i = 0;i< BUFFER_SIZE;i++)
		{
			*(__IO uint16_t *) (SDRAM_DEVICE_ADDR+i*2) =writeBuffer16[i];
		}
		uint16_t readBuffer0[BUFFER_SIZE];
		memset(readBuffer0,0,BUFFER_SIZE);
		//SDRAM Read Data
		//HAL_SDRAM_Read_8b(&hsdram1,(uint32_t *)addrx,readBuffer0,BUFFER_SIZE);
		for(int i = 0;i< BUFFER_SIZE;i++)
			readBuffer0[i] = *(__IO uint16_t *) (SDRAM_DEVICE_ADDR+i*2 );
		//HAL_SDRAM_Read_32b(&_HSDRAM,(uint32_t *)addrx,readBuffer0,BUFFER_SIZE);
		char part[10];
		for(int i = 0;i< BUFFER_SIZE;i++)
		{
			memset(part,0,10);
			sprintf(part,"SDRam[%02d]:%04x",i,readBuffer0[i]);
			Debug(part);
		}
}
void SDRam8bTest()
{
	#define BUFFER_SIZE  15
	HAL_SDRAM_WriteProtection_Disable(&hsdram1);
	uint8_t writeBuffer[BUFFER_SIZE] = {0xAA,0x55,0xAA,0x55,0xAA, \
																			 0x55,0xAA,0x55,0xAA,0x55, \
																			 0xAA,0x55,0xAA,0x55,0xAA};
	uint32_t addrx = SDRAM_DEVICE_ADDR; 
																			 
	for(int i = 0;i< BUFFER_SIZE;i++)
	{
		*(__IO uint8_t *) (SDRAM_DEVICE_ADDR+i) = writeBuffer[i];
	}
	uint8_t readBuffer0[BUFFER_SIZE];
	memset(readBuffer0,0,BUFFER_SIZE);
	//SDRAM Read Data
	
	for(int i = 0;i< BUFFER_SIZE;i++)
		readBuffer0[i] = *(__IO uint8_t *) (SDRAM_DEVICE_ADDR+i );
	
	char part[10];
	for(int i = 0;i< BUFFER_SIZE;i++)
	{
		memset(part,0,10);
		sprintf(part,"SDRam[%02d]:%02x",i,readBuffer0[i]);
		Debug(part);
	}
}
void testExFlash()
{
	uint16_t flash_id = SPI_Flash_ReadID();
	char flashmsg[100];
	sprintf(flashmsg,"Flash ID is : %08x",flash_id);
	Debug(flashmsg);
	uint16_t flash_status = SPI_Flash_ReadSR();
	char flashmsg2[100];
	sprintf(flashmsg2,"Flash Status is : %02x",flash_status);
	Debug(flashmsg2);
}
void writeExFlash()
{
	//SPI_Flash_Erase_Chip();
	uint8_t testdata[16] = {0x01,0x02,0x33,0x23,0x18,0x25,0x02,0x33,0x3a,0x20,0x24,0x21,0x22,0x91,0x21	};
	SPI_Flash_Write(0x0 ,16 , testdata);

	uint8_t revData[20];
	memset(revData,0,20);
	SPI_Flash_Read( 0x0 ,16,revData);

	char debugmsg[100];
	memset(debugmsg,0,100);
	char datas[80];
	memset(datas,0,80);
	sprintf(debugmsg,"ExFlash read data:");
	char datax[10];
	for(int i = 0;i< 16;i++)
	{
		memset(datax,0,10);
		sprintf(datax,"%02xh,",revData[i]);
		sprintf(datas,"%s%s",datas,datax);
	}
	sprintf(debugmsg,"%s %s",debugmsg,datas);
	Debug(debugmsg);
}

FRESULT scan_files (char* path);

void writeTFCard(FATFS *fatfs,char *path)
{
		FIL file;
		FRESULT res;
		char tname[20];

		sprintf(tname,"%sdata.txt",path);
		//Debug(tname);
		res = f_open(&file, tname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
		if(res!=FR_OK)
		{
			char msg[20];
			sprintf(msg,"err return is %d",res);
			
			Debug(msg);
			uartWrite("f_open() fail .. \r\n");
		}
		
		//else{
			//uartWrite("f_open() success .. \r\n");
	//	}
		res = f_lseek(&file,file.fsize);	

		uint8_t n = f_puts("hello world ..\r\n", &file) ;  //向文件末写入字符串
											
		if(n<1)  //判断写是否成功                        
		{                                                      
			uartWrite("f_puts() fail .. \r\n");                                             
		}
		f_close(&file);
		
#if F_READ

	while(1){									 //使用f_read读文件
		res = f_read(&file, buffer, 1, &br);     //一次读一个字节知道读完全部文件信息

		if (res == FR_OK )
		{
			printf("%s",buffer);
		}else{
		    printf("\r\n f_read() fail .. \r\n");	
		}

		if(f_eof(&file)) {break;}
	}

#endif
	
	
#if F_UNLINK

	res = f_unlink("test.jpg");	   //前提SD下存在一个test.jpg

	if(res!=FR_OK)
	{
		printf("\r\n f_unlink() fail .. \r\n");
	}else{
		printf("\r\n f_unlink() success .. \r\n");
	}

#endif

#if SCAN_FILES

	printf("\r\n the directory files : \r\n");
	scan_files("/");	      //扫描根目录

#endif
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (also used as work area) */
)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("\r\n %s/%s \r\n", path, fn);
            }
        }
    }

    return res;
}

