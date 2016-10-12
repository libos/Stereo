/**
  ******************************************************************************
  * @file    exflash_diskio.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    22-April-2014
  * @brief   Header for exflash_diskio.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXFLASH_DISKIO_H
#define __EXFLASH_DISKIO_H

#include <string.h>
#include "ff_gen_drv.h"
#include "dataflash.h"


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
// extern Diskio_drvTypeDef  SD_Driver;
/* Block Size in Bytes */
#define W25X16_BLOCK_SIZE 512
#define W25X16_BLOCK_NUM 4096

DSTATUS EXFLASH_initialize(void);
DRESULT EXFLASH_read(BYTE *buff, DWORD sector, BYTE count);
DSTATUS EXFLASH_status(void);
#if _USE_WRITE == 1
DRESULT EXFLASH_write(const BYTE *buff, DWORD sector, BYTE count);
#endif
#if _USE_IOCTL == 1
DRESULT EXFLASH_ioctl(BYTE cmd, void *buff);
#endif



#endif /* __EXFLASH_DISKIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

