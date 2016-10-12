/**
 ******************************************************************************
  * @file    bsp_driver_cam.h
  * @author  Liber Liu
  * @version V1.0.0
  * @date    30/11/2014 22:02:34
  * @brief   This file contains the common defines and functions prototypes for  
  *          the bsp_driver_cam.c driver.
	* 				 MT9M111 drivers.
  ******************************************************************************
**/

#ifndef __STM32F4XX_CAMERA_H
#define __STM32F4XX_CAMERA_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>

// Camera Address
#define MT1ADDR 0x48
#define MT2ADDR 0x5D
#define CAMERA1 MT1ADDR
#define CAMERA2 MT2ADDR
/*
 * Sensor core register addresses (0x000..0x0ff)
 */
#define MT9M111_CHIP_VERSION		0x000
#define MT9M111_ROW_START		0x001
#define MT9M111_COLUMN_START		0x002
#define MT9M111_WINDOW_HEIGHT		0x003
#define MT9M111_WINDOW_WIDTH		0x004
#define MT9M111_HORIZONTAL_BLANKING_B	0x005
#define MT9M111_VERTICAL_BLANKING_B	0x006
#define MT9M111_HORIZONTAL_BLANKING_A	0x007
#define MT9M111_VERTICAL_BLANKING_A	0x008
#define MT9M111_SHUTTER_WIDTH		0x009
#define MT9M111_ROW_SPEED		0x00a
#define MT9M111_EXTRA_DELAY		0x00b
#define MT9M111_SHUTTER_DELAY		0x00c
#define MT9M111_RESET			0x00d
#define MT9M111_READ_MODE_B		0x020
#define MT9M111_READ_MODE_A		0x021
#define MT9M111_FLASH_CONTROL		0x023
#define MT9M111_GREEN1_GAIN		0x02b
#define MT9M111_BLUE_GAIN		0x02c
#define MT9M111_RED_GAIN		0x02d
#define MT9M111_GREEN2_GAIN		0x02e
#define MT9M111_GLOBAL_GAIN		0x02f
#define MT9M111_CONTEXT_CONTROL		0x0c8
#define MT9M111_PAGE_MAP		0x0f0
#define MT9M111_BYTE_WISE_ADDR		0x0f1

#define MT9M111_RESET_SYNC_CHANGES	(1 << 15)
#define MT9M111_RESET_RESTART_BAD_FRAME	(1 << 9)
#define MT9M111_RESET_SHOW_BAD_FRAMES	(1 << 8)
#define MT9M111_RESET_RESET_SOC		(1 << 5)
#define MT9M111_RESET_OUTPUT_DISABLE	(1 << 4)
#define MT9M111_RESET_CHIP_ENABLE	(1 << 3)
#define MT9M111_RESET_ANALOG_STANDBY	(1 << 2)
#define MT9M111_RESET_RESTART_FRAME	(1 << 1)
#define MT9M111_RESET_RESET_MODE	(1 << 0)

#define MT9M111_RM_FULL_POWER_RD	(0 << 10)
#define MT9M111_RM_LOW_POWER_RD		(1 << 10)
#define MT9M111_RM_COL_SKIP_4X		(1 << 5)
#define MT9M111_RM_ROW_SKIP_4X		(1 << 4)
#define MT9M111_RM_COL_SKIP_2X		(1 << 3)
#define MT9M111_RM_ROW_SKIP_2X		(1 << 2)
#define MT9M111_RMB_MIRROR_COLS		(1 << 1)
#define MT9M111_RMB_MIRROR_ROWS		(1 << 0)
#define MT9M111_CTXT_CTRL_RESTART	(1 << 15)
#define MT9M111_CTXT_CTRL_DEFECTCOR_B	(1 << 12)
#define MT9M111_CTXT_CTRL_RESIZE_B	(1 << 10)
#define MT9M111_CTXT_CTRL_CTRL2_B	(1 << 9)
#define MT9M111_CTXT_CTRL_GAMMA_B	(1 << 8)
#define MT9M111_CTXT_CTRL_XENON_EN	(1 << 7)
#define MT9M111_CTXT_CTRL_READ_MODE_B	(1 << 3)
#define MT9M111_CTXT_CTRL_LED_FLASH_EN	(1 << 2)
#define MT9M111_CTXT_CTRL_VBLANK_SEL_B	(1 << 1)
#define MT9M111_CTXT_CTRL_HBLANK_SEL_B	(1 << 0)

/*
 * Colorpipe register addresses (0x100..0x1ff)
 */
#define MT9M111_OPER_MODE_CTRL		0x106
#define MT9M111_OUTPUT_FORMAT_CTRL	0x108
#define MT9M111_REDUCER_XZOOM_B		0x1a0
#define MT9M111_REDUCER_XSIZE_B		0x1a1
#define MT9M111_REDUCER_YZOOM_B		0x1a3
#define MT9M111_REDUCER_YSIZE_B		0x1a4
#define MT9M111_REDUCER_XZOOM_A		0x1a6
#define MT9M111_REDUCER_XSIZE_A		0x1a7
#define MT9M111_REDUCER_YZOOM_A		0x1a9
#define MT9M111_REDUCER_YSIZE_A		0x1aa

#define MT9M111_OUTPUT_FORMAT_CTRL2_A	0x13a
#define MT9M111_OUTPUT_FORMAT_CTRL2_B	0x19b

#define MT9M111_OPMODE_AUTOEXPO_EN	(1 << 14)
#define MT9M111_OPMODE_AUTOWHITEBAL_EN	(1 << 1)
#define MT9M111_OUTFMT_FLIP_BAYER_COL	(1 << 9)
#define MT9M111_OUTFMT_FLIP_BAYER_ROW	(1 << 8)
#define MT9M111_OUTFMT_PROCESSED_BAYER	(1 << 14)
#define MT9M111_OUTFMT_BYPASS_IFP	(1 << 10)
#define MT9M111_OUTFMT_INV_PIX_CLOCK	(1 << 9)
#define MT9M111_OUTFMT_RGB		(1 << 8)
#define MT9M111_OUTFMT_RGB565		(0 << 6)
#define MT9M111_OUTFMT_RGB555		(1 << 6)
#define MT9M111_OUTFMT_RGB444x		(2 << 6)
#define MT9M111_OUTFMT_RGBx444		(3 << 6)
#define MT9M111_OUTFMT_TST_RAMP_OFF	(0 << 4)
#define MT9M111_OUTFMT_TST_RAMP_COL	(1 << 4)
#define MT9M111_OUTFMT_TST_RAMP_ROW	(2 << 4)
#define MT9M111_OUTFMT_TST_RAMP_FRAME	(3 << 4)
#define MT9M111_OUTFMT_SHIFT_3_UP	(1 << 3)
#define MT9M111_OUTFMT_AVG_CHROMA	(1 << 2)
#define MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN	(1 << 1)
#define MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr_RGB_R_B	(1 << 0)

#define RESO_1280x1024	0x00
#define	RESO_640x512		0x01
#define RESO_640x480		0x02
#define RESO_320x240		0x03
#define RESO_160x120		0x04

#define RESO_SXGA 		RESO_1280x1024
#define RESO_QSXGA		RESO_640x512
#define RESO_VGA			RESO_640x480
#define RESO_QVGA			RESO_320x240
#define RESO_CIF		 	RESO_QVGA
#define RESO_QCIF			RESO_160x120

#define RESO_BIG			RESO_SXGA
#define RESO_SMALL		RESO_QSXGA


#define CAM1_RESET GPIO_PIN_12
#define CAM2_RESET GPIO_PIN_13

#define CAM1_ON() 	HAL_GPIO_WritePin(GPIOB,CAM1_RESET,GPIO_PIN_SET);
#define CAM1_OFF() 	HAL_GPIO_WritePin(GPIOB,CAM1_RESET,GPIO_PIN_RESET);
#define CAM2_ON()		HAL_GPIO_WritePin(GPIOB,CAM2_RESET,GPIO_PIN_SET);
#define CAM2_OFF() 	HAL_GPIO_WritePin(GPIOB,CAM2_RESET,GPIO_PIN_RESET);


#define CAM1_ENABLE() 	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
#define CMA1_DISABLE()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
#define CAM2_ENABLE()		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
#define CMA2_DISABLE()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
#define MAX3372_ENABLE()	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);


#define CAM1_EN() CAM1_ENABLE();CMA1_DISABLE();CAM1_ON();CAM2_OFF();HAL_Delay(100)
#define CAM2_EN() CMA1_DISABLE();CAM2_ENABLE();CAM1_OFF();CAM2_ON();HAL_Delay(100)

#define CAM_OK 0x00
#define CAM_ERR 0x01

#define MT9M111_MIN_DARK_ROWS	8
#define MT9M111_MIN_DARK_COLS	26
#define MT9M111_MAX_HEIGHT	1024
#define MT9M111_MAX_WIDTH	1280
#define u16 uint16_t

enum mbus_pixelcode {
	MBUS_FMT_FIXED = 0x0001,

	/* RGB - next is 0x100e */
	MBUS_FMT_RGB444_2X8_PADHI_BE = 0x1001,
	MBUS_FMT_RGB444_2X8_PADHI_LE = 0x1002,
	MBUS_FMT_RGB555_2X8_PADHI_BE = 0x1003,
	MBUS_FMT_RGB555_2X8_PADHI_LE = 0x1004,
	MBUS_FMT_BGR565_2X8_BE = 0x1005,
	MBUS_FMT_BGR565_2X8_LE = 0x1006,
	MBUS_FMT_RGB565_2X8_BE = 0x1007,
	MBUS_FMT_RGB565_2X8_LE = 0x1008,
	MBUS_FMT_RGB666_1X18 = 0x1009,
	MBUS_FMT_RGB888_1X24 = 0x100a,
	MBUS_FMT_RGB888_2X12_BE = 0x100b,
	MBUS_FMT_RGB888_2X12_LE = 0x100c,
	MBUS_FMT_ARGB8888_1X32 = 0x100d,

	/* YUV (including grey) - next is 0x2024 */
	MBUS_FMT_Y8_1X8 = 0x2001,
	MBUS_FMT_UV8_1X8 = 0x2015,
	MBUS_FMT_UYVY8_1_5X8 = 0x2002,
	MBUS_FMT_VYUY8_1_5X8 = 0x2003,
	MBUS_FMT_YUYV8_1_5X8 = 0x2004,
	MBUS_FMT_YVYU8_1_5X8 = 0x2005,
	MBUS_FMT_UYVY8_2X8 = 0x2006,
	MBUS_FMT_VYUY8_2X8 = 0x2007,
	MBUS_FMT_YUYV8_2X8 = 0x2008,
	MBUS_FMT_YVYU8_2X8 = 0x2009,
	MBUS_FMT_Y10_1X10 = 0x200a,
	MBUS_FMT_UYVY10_2X10 = 0x2018,
	MBUS_FMT_VYUY10_2X10 = 0x2019,
	MBUS_FMT_YUYV10_2X10 = 0x200b,
	MBUS_FMT_YVYU10_2X10 = 0x200c,
	MBUS_FMT_Y12_1X12 = 0x2013,
	MBUS_FMT_UYVY8_1X16 = 0x200f,
	MBUS_FMT_VYUY8_1X16 = 0x2010,
	MBUS_FMT_YUYV8_1X16 = 0x2011,
	MBUS_FMT_YVYU8_1X16 = 0x2012,
	MBUS_FMT_YDYUYDYV8_1X16 = 0x2014,
	MBUS_FMT_UYVY10_1X20 = 0x201a,
	MBUS_FMT_VYUY10_1X20 = 0x201b,
	MBUS_FMT_YUYV10_1X20 = 0x200d,
	MBUS_FMT_YVYU10_1X20 = 0x200e,
	MBUS_FMT_YUV10_1X30 = 0x2016,
	MBUS_FMT_AYUV8_1X32 = 0x2017,
	MBUS_FMT_UYVY12_2X12 = 0x201c,
	MBUS_FMT_VYUY12_2X12 = 0x201d,
	MBUS_FMT_YUYV12_2X12 = 0x201e,
	MBUS_FMT_YVYU12_2X12 = 0x201f,
	MBUS_FMT_UYVY12_1X24 = 0x2020,
	MBUS_FMT_VYUY12_1X24 = 0x2021,
	MBUS_FMT_YUYV12_1X24 = 0x2022,
	MBUS_FMT_YVYU12_1X24 = 0x2023,

	/* Bayer - next is 0x3019 */
	MBUS_FMT_SBGGR8_1X8 = 0x3001,
	MBUS_FMT_SGBRG8_1X8 = 0x3013,
	MBUS_FMT_SGRBG8_1X8 = 0x3002,
	MBUS_FMT_SRGGB8_1X8 = 0x3014,
	MBUS_FMT_SBGGR10_ALAW8_1X8 = 0x3015,
	MBUS_FMT_SGBRG10_ALAW8_1X8 = 0x3016,
	MBUS_FMT_SGRBG10_ALAW8_1X8 = 0x3017,
	MBUS_FMT_SRGGB10_ALAW8_1X8 = 0x3018,
	MBUS_FMT_SBGGR10_DPCM8_1X8 = 0x300b,
	MBUS_FMT_SGBRG10_DPCM8_1X8 = 0x300c,
	MBUS_FMT_SGRBG10_DPCM8_1X8 = 0x3009,
	MBUS_FMT_SRGGB10_DPCM8_1X8 = 0x300d,
	MBUS_FMT_SBGGR10_2X8_PADHI_BE = 0x3003,
	MBUS_FMT_SBGGR10_2X8_PADHI_LE = 0x3004,
	MBUS_FMT_SBGGR10_2X8_PADLO_BE = 0x3005,
	MBUS_FMT_SBGGR10_2X8_PADLO_LE = 0x3006,
	MBUS_FMT_SBGGR10_1X10 = 0x3007,
	MBUS_FMT_SGBRG10_1X10 = 0x300e,
	MBUS_FMT_SGRBG10_1X10 = 0x300a,
	MBUS_FMT_SRGGB10_1X10 = 0x300f,
	MBUS_FMT_SBGGR12_1X12 = 0x3008,
	MBUS_FMT_SGBRG12_1X12 = 0x3010,
	MBUS_FMT_SGRBG12_1X12 = 0x3011,
	MBUS_FMT_SRGGB12_1X12 = 0x3012,

	/* JPEG compressed formats - next is 0x4002 */
	MBUS_FMT_JPEG_1X8 = 0x4001,

	/* Vendor specific formats - next is 0x5002 */

	/* S5C73M3 sensor specific interleaved UYVY and JPEG */
	MBUS_FMT_S5C_UYVY_JPEG_1X8 = 0x5001,

	/* HSV - next is 0x6002 */
	MBUS_FMT_AHSV8888_1X32 = 0x6001,
};
/* see also http://vektor.theorem.ca/graphics/ycbcr/ */
enum colorspace {
	/* ITU-R 601 -- broadcast NTSC/PAL */
	COLORSPACE_SMPTE170M     = 1,

	/* 1125-Line (US) HDTV */
	COLORSPACE_SMPTE240M     = 2,

	/* HD and modern captures. */
	COLORSPACE_REC709        = 3,

	/* broken BT878 extents (601, luma range 16-253 instead of 16-235) */
	COLORSPACE_BT878         = 4,

	/* These should be useful.  Assume 601 extents. */
	COLORSPACE_470_SYSTEM_M  = 5,
	COLORSPACE_470_SYSTEM_BG = 6,

	/* I know there will be cameras that send this.  So, this is
	 * unspecified chromaticities and full 0-255 on each of the
	 * Y'CbCr components
	 */
	COLORSPACE_JPEG          = 7,

	/* For RGB colourspaces, this is probably a good start. */
	COLORSPACE_SRGB          = 8,
};
struct mbus_framefmt {
	uint32_t			width;
	uint32_t			height;
	uint32_t			code;
	uint32_t			field;
	uint32_t			colorspace;
	uint32_t			reserved[7];
};
struct mt9m111_datafmt {
	enum mbus_pixelcode	code;
	enum colorspace		colorspace;
};
static const struct mt9m111_datafmt mt9m111_colour_fmts[] = {
	{MBUS_FMT_YUYV8_2X8, COLORSPACE_JPEG},
	{MBUS_FMT_YVYU8_2X8, COLORSPACE_JPEG},
	{MBUS_FMT_UYVY8_2X8, COLORSPACE_JPEG},
	{MBUS_FMT_VYUY8_2X8, COLORSPACE_JPEG},
	{MBUS_FMT_RGB555_2X8_PADHI_LE, COLORSPACE_SRGB},
	{MBUS_FMT_RGB555_2X8_PADHI_BE, COLORSPACE_SRGB},
	{MBUS_FMT_RGB565_2X8_LE, COLORSPACE_SRGB},
	{MBUS_FMT_RGB565_2X8_BE, COLORSPACE_SRGB},
	{MBUS_FMT_BGR565_2X8_LE, COLORSPACE_SRGB},
	{MBUS_FMT_BGR565_2X8_BE, COLORSPACE_SRGB},
	{MBUS_FMT_SBGGR8_1X8, COLORSPACE_SRGB},
	{MBUS_FMT_SBGGR10_2X8_PADHI_LE, COLORSPACE_SRGB},
};
struct mt9m111_context {
	u16 read_mode;
	u16 blanking_h;
	u16 blanking_v;
	u16 reducer_xzoom;
	u16 reducer_yzoom;
	u16 reducer_xsize;
	u16 reducer_ysize;
	u16 output_fmt_ctrl2;
	u16 control;
};
static struct mt9m111_context context_a = {
	.read_mode		= MT9M111_READ_MODE_A,
	.blanking_h		= MT9M111_HORIZONTAL_BLANKING_A,
	.blanking_v		= MT9M111_VERTICAL_BLANKING_A,
	.reducer_xzoom		= MT9M111_REDUCER_XZOOM_A,
	.reducer_yzoom		= MT9M111_REDUCER_YZOOM_A,
	.reducer_xsize		= MT9M111_REDUCER_XSIZE_A,
	.reducer_ysize		= MT9M111_REDUCER_YSIZE_A,
	.output_fmt_ctrl2	= MT9M111_OUTPUT_FORMAT_CTRL2_A,
	.control		= MT9M111_CTXT_CTRL_RESTART,
};
static struct mt9m111_context context_b = {
	.read_mode		= MT9M111_READ_MODE_B,
	.blanking_h		= MT9M111_HORIZONTAL_BLANKING_B,
	.blanking_v		= MT9M111_VERTICAL_BLANKING_B,
	.reducer_xzoom		= MT9M111_REDUCER_XZOOM_B,
	.reducer_yzoom		= MT9M111_REDUCER_YZOOM_B,
	.reducer_xsize		= MT9M111_REDUCER_XSIZE_B,
	.reducer_ysize		= MT9M111_REDUCER_YSIZE_B,
	.output_fmt_ctrl2	= MT9M111_OUTPUT_FORMAT_CTRL2_B,
	.control		= MT9M111_CTXT_CTRL_RESTART |
		MT9M111_CTXT_CTRL_DEFECTCOR_B | MT9M111_CTXT_CTRL_RESIZE_B |
		MT9M111_CTXT_CTRL_CTRL2_B | MT9M111_CTXT_CTRL_GAMMA_B |
		MT9M111_CTXT_CTRL_READ_MODE_B | MT9M111_CTXT_CTRL_VBLANK_SEL_B |
		MT9M111_CTXT_CTRL_HBLANK_SEL_B,
};

struct rect {
	int32_t   left;
	int32_t   top;
	uint32_t   width;
	uint32_t   height;
};

extern void i2cWrite(uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t tx);
extern void i2cRead(uint8_t I2C_ADDRESS, uint8_t RegAddr, uint8_t *aRxBuffer, uint8_t rx);
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define assemble_tx(reg,data) prepend_array(reg,data)
#define only_reg(reg) reg & 0xff

#define data_write(camera, reg, data)  i2cWrite(camera, assemble_tx(reg,data), 3)
#define reg_write(camera, reg) i2cWrite(camera, reg, 1)
#define reg_read(camera, reg, data)  i2cRead(camera, only_reg(reg), data, (sizeof(data)/sizeof(data[0])-1))

uint8_t *prepend_array(uint16_t reg,uint16_t data);
uint8_t set_reg_page(uint8_t camera,uint16_t reg);
uint8_t mt_reg_set(uint8_t camera,uint16_t reg,uint16_t data);
uint8_t mt_reg_clear(uint8_t camera,uint16_t reg,uint16_t data);
uint8_t mt_reg_mask(uint8_t camera,uint16_t reg,uint16_t mask , uint16_t data);
uint32_t GetSize(uint8_t reso);

uint8_t setup_rect(uint8_t camera, struct mt9m111_context *ctx, struct rect *rect,
											 unsigned int width, unsigned int height);


#ifdef __cplusplus
}
#endif


#endif
