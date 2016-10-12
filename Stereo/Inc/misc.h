#ifndef __STEREO_MISC_H
#define __STEREO_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "stdio.h"
#include "string.h"
#include "dataflash.h"

void writeTFCard(FATFS *fatfs,char *path);

void testExFlash(void);
void writeExFlash(void);
	
void SDRam16bTest(void);
void SDRam8bTest(void);
void DMA_TEST(void);
	
extern SDRAM_HandleTypeDef hsdram1;
extern void Debug(char *msg);
extern void uartWrite(void *msg);

#ifdef __cplusplus
}
#endif

#endif

