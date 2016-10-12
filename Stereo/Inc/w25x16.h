#ifndef W25X16_H 
#define W25X16_H 

//PIN discribe
//#define F_HLD PB11
//#define F_CS PB12
//#define F_CLK PB13
//#define F_DO PB14
//#define F_DI PB15

//#define Set_F_HLD  GPIOB->BSRR  = 0x00000800;  //PB11
//#define Clr_F_HLD  GPIOB->BRR   = 0x00000800;

//#define Set_F_CS  GPIOB->BSRR  = 0x00001000;   //PB12
//#define Clr_F_CS  GPIOB->BRR   = 0x00001000;


//操作码定义 
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_BlockErase				0xD8 
#define W25X_SectorErase			0x20 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F


#endif 
