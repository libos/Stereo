#ifndef DATAFLASH_H 
#define DATAFLASH_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

#define  EXFLASH_OK         0x01
#define  EXFLASH_ERROR      0x00

#define MAX_ADDR		0x1FFFFF	//定义芯片内部最大地址
#define DF_PAGE_SIZE    512
#define DF_READ_BUFFER  1 
#define DF_WRITE_BUFFER 0

typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;


void df_read_open(uint32_t addr); 
void df_write_open(uint32_t addr); 
void df_read(uint8_t *buf,uint8_t size); 
void df_write(uint8_t *buf,uint8_t size); 
void df_read_seek(uint32_t addr); 
void df_write_seek(uint32_t addr); 
void df_read_close(void); 
void df_write_close(void);

uint8_t spi_write(uint8_t data);
uint8_t spi_read(void);
uint8_t EXFLASH_INIT(void);

uint8_t SPI_Flash_Read(uint32_t Dst, uint32_t NByte,uint8_t* RcvBufPt);
uint16_t SPI_Flash_ReadID(void);
//uint32_t SPI_Flash_ReadID(void);
uint8_t SPI_Flash_Write(uint32_t Dst,uint32_t NByte,uint8_t* SndbufPt);
void SPI_Flash_Erase_Chip(void);
void SPI_FLASH_Write_Enable(void);
void SPI_Flash_Wait_Busy(void);
uint8_t SPI_Flash_ReadSR(void);
uint8_t SPI_Flash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_Flash_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite); 

#endif 
