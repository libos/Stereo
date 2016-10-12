/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @date    30/10/2014 20:02:35
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

/* External variables --------------------------------------------------------*/

extern void Debug(char *msg);
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_dcmi;
extern DMA_HandleTypeDef DmaHandle;
extern DCMI_HandleTypeDef hdcmi;
/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/**
* @brief This function handles DMA2 Stream1 global interrupt.
*/
void DMA2_Stream1_IRQHandler(void)
{
  HAL_NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);
  HAL_DMA_IRQHandler(&hdma_dcmi);
}
void HardFault_Handler(void)
{
		Debug("HardFault");
}
void DMA2_Stream0_IRQHandler(void)
{
	 HAL_DMA_IRQHandler(&DmaHandle);
}
/**
* @brief This function handles DCMI global interrupt.
*/
void DCMI_IRQHandler(void)
{
  HAL_NVIC_ClearPendingIRQ(DCMI_IRQn);
  HAL_DCMI_IRQHandler(&hdcmi);
}

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
  HAL_NVIC_ClearPendingIRQ(OTG_FS_IRQn);
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
