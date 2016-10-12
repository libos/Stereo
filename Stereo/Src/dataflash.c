
#include "W25X16.h"
#include "dataflash.h"


#include "stdio.h"
extern void Debug(char *msg);
/*****下面是全局变量定义******/
static uint32_t CurReadAddr;	//当前读的地址
static uint32_t CurWriteAddr;	//当前写页地址
/*****下面是内部调用的接口函数******/
extern SPI_HandleTypeDef hspi5;
//HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
//HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
//HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
//HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi);

#define FLASH_CS_ENABLE()  		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_RESET);
#define FLASH_CS_DISABLE()		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_SET);

uint8_t EXFLASH_INIT(void)
{
	static uint8_t flashstatus = EXFLASH_OK;
	
	return flashstatus;
}

//从SPI口输出一字节数据
uint8_t spi_write(uint8_t data)
{
	uint8_t rev;
  /* Send byte through the SPI2 peripheral */
  HAL_SPI_TransmitReceive(&hspi5,&data,&rev,1,DEFTIMEOUT);
  /* Return the byte read from the SPI bus */
  return rev;
}
uint8_t spi_read()
{
	uint8_t rev;
  /* Send byte through the SPI2 peripheral */
  HAL_SPI_Receive(&hspi5,&rev,1,DEFTIMEOUT);
  /* Return the byte read from the SPI bus */
  return rev;
}

uint16_t SPI_Flash_ReadID(void)
{
	uint16_t Temp = 0;
	FLASH_CS_ENABLE();
	spi_write(0x90);//发送读取ID命令  
	spi_write(0x00);   
	spi_write(0x00);   
	spi_write(0x00);
	Temp|=spi_write(0xFF)<<8;
	Temp|=spi_write(0xFF);
	FLASH_CS_DISABLE();
	return Temp;
}
uint8_t SPI_Flash_ReadSR(void)   
{  
	uint8_t byte=0;
	FLASH_CS_ENABLE();
	uint8_t ret = spi_write(0x05);//W25X_ReadStatusReg);    //发送读取状态寄存器命令
	byte= spi_write(0xFF);             //读取一个字节
	FLASH_CS_DISABLE();
	return byte;   
} 
uint8_t SPI_Flash_Read(uint32_t Dst, uint32_t NByte,uint8_t* RcvBufPt)   
{ 
 	uint32_t i = 0;
  uint8_t temp;
	if ((Dst+NByte > MAX_ADDR)||(NByte == 0))	
		return (ERROR);	 //	检查入口参数
	
	FLASH_CS_ENABLE();
  spi_write(W25X_ReadData);         //发送读取命令   
  spi_write((uint8_t)((Dst)>>16));  //发送24bit地址    
  spi_write((uint8_t)((Dst)>>8));   
  spi_write((uint8_t)Dst);   
  for(i=0;i<NByte;i++)
	{ 
    temp = spi_write(0xFF);   //循环读数 
    RcvBufPt[i]=temp;
#if 0
		char msg[20];
		sprintf(msg,"read data:%02x",temp);
		Debug(msg);
#endif
  }
	FLASH_CS_DISABLE();
	return 1;        
}	


/************************************************************************
** 函数名称:SSTF016B_WR											
** 函数功能:SST25VF016B的写函数，可写1个和多个数据到指定地址									
** 入口参数:
**			uint32 Dst：目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）				
**			uint8* SndbufPt:发送缓存区指针
**      	uint32 NByte:要写的数据字节数
** 出口参数:操作成功则返回OK,失败则返回ERROR		
** 注	 意:若某功能下,某一入口参数无效,可在该入口参数处填Invalid，该参数将被忽略
************************************************************************/
uint8_t SPI_Flash_Write(uint32_t Dst,uint32_t NByte,uint8_t* SndbufPt)
{
  uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;

	if (( (Dst+NByte-1 > MAX_ADDR)||(NByte == 0) ))
	{
		return (0);	 //	检查入口参数
	}

	secpos = Dst / 4096;										//扇区地址 0~511 for w25x16
	secoff = Dst % 4096;										//在扇区内的偏移
	secremain = 4096 - secoff;							//扇区剩余空间大小   

	if( NByte <= secremain )
		secremain = NByte;											//不大于4096个字节
	while(1) 
	{	
		SPI_Flash_Write_NoCheck(SndbufPt, Dst , secremain);//写已经擦除了的,直接写入扇区剩余区间. 			 
		if(NByte == secremain)
			break;							//写入结束了
		else									//写入未结束
		{
			secpos++;						//扇区地址增1
			secoff=0;						//偏移位置为0 	 

	SndbufPt+=secremain;  		//指针偏移
			Dst+=secremain;						//写地址偏移 
	NByte-=secremain;					//字节数递减
			if(NByte>4096)
				secremain=4096;					//下一个扇区还是写不完
			else 
				secremain=NByte;				//下一个扇区可以写完了
		}	 
	}	
  
	return (1);		
}

uint8_t SPI_Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;
	pageremain = 256 - WriteAddr % 256; //单页剩余的字节数	 
	if(NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite;//不大于256个字节
	while(1)
	{ 
		SPI_Flash_Write_Page ( pBuffer, WriteAddr, pageremain );
		if(NumByteToWrite == pageremain)
			break;										//写入结束了
	 	else 												//NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;		//减去已经写入了的字节数
			if(NumByteToWrite > 256)
				pageremain = 256; //一次可以写入256个字节
			else 
				pageremain=NumByteToWrite; //不够256个字节了
		}
	}
	return 1;
}

void SPI_Flash_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
  SPI_FLASH_Write_Enable();                 	 //SET WEL 
  FLASH_CS_ENABLE();
	spi_write(W25X_PageProgram);      					//发送写页命令   
  spi_write((uint8_t)((WriteAddr)>>16)); 			//发送24bit地址    
  spi_write((uint8_t)((WriteAddr)>>8));   
  spi_write((uint8_t)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)
	{
		uint8_t ret = spi_write(pBuffer[i]);//循环写数
#if 0
		char msg[20];
		sprintf(msg,"puts data:%02x",ret);
		Debug(msg);
#endif
	}
	FLASH_CS_DISABLE();
	SPI_Flash_Wait_Busy();				 //等待写入结束
}  

void SPI_Flash_Erase_Chip(void)   
{                                             
  SPI_FLASH_Write_Enable();                  //SET WEL 
  SPI_Flash_Wait_Busy();
	FLASH_CS_ENABLE();
  spi_write(W25X_ChipErase);        //发送片擦除命令
	FLASH_CS_DISABLE();
	SPI_Flash_Wait_Busy();   			 //等待芯片擦除结束
} 

void SPI_FLASH_Write_Enable(void)   
{
	FLASH_CS_ENABLE();
  spi_write(W25X_WriteEnable);      //发送写使能
	FLASH_CS_DISABLE();
} 

void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  



/*****下面是为外部调用而提供的接口函数******/


//读初始化功能函数,addr为打开后读到的初始地址
void df_read_open(uint32_t addr)
{
	CurReadAddr=addr;	//当前读的地址
}

void df_write_open(uint32_t addr)
{
	CurWriteAddr=addr;	//当前写页地址
}

void df_read(uint8_t *buf,uint8_t size)
{
  if( CurReadAddr + size <= MAX_ADDR )
  {
    SPI_Flash_Read(CurReadAddr,size,buf);
    CurReadAddr+=size;
  }
}

void df_write(uint8_t *buf,uint8_t size)
{
  if( CurWriteAddr + size <= MAX_ADDR )
  {
    SPI_Flash_Write(CurWriteAddr,size,buf);
    CurWriteAddr+=size;
  }
}


//调整写指针
void df_read_seek(uint32_t addr)
{
 df_read_close();
 df_read_open(addr);
}

//调整读指针
void df_write_seek(uint32_t addr)
{
 df_write_close();
 df_write_open(addr); 
}

void df_read_close(void)
{

}

void df_write_close(void)
{
	
}
