/**
 ******************************************************************************
  * @file    bsp_driver_sdram.c (based on stm324x9i_eval_sdram.c)
  * @author  MCD Teams
  * @version V1.0.0
  * @date    30/10/2014 20:02:34
  * @brief   This file includes a generic SDRAM driver.
  ******************************************************************************
**/
#include "bsp_driver_cam.h"

const struct mt9m111_datafmt *find_datafmt(enum mbus_pixelcode code)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(mt9m111_colour_fmts); i++)
		if (mt9m111_colour_fmts[i].code == code)
			return mt9m111_colour_fmts + i;

	return mt9m111_colour_fmts;
}

uint8_t setup_rect(uint8_t camera, struct mt9m111_context *ctx, struct rect *rect,
											 unsigned int width, unsigned int height)
{	
	data_write(camera, ctx->reducer_xzoom, rect->width);
	data_write(camera, ctx->reducer_yzoom, rect->height);
	data_write(camera, ctx->reducer_xsize, width);
	data_write(camera, ctx->reducer_ysize, height);
	return CAM_OK;
}
uint8_t setup_geometry(uint8_t camera, struct rect *rect,
			int width, int height, enum mbus_pixelcode code)
{
	data_write(camera, MT9M111_COLUMN_START, rect->left);
	data_write(camera,MT9M111_ROW_START, rect->top);
  data_write(camera,MT9M111_WINDOW_WIDTH, rect->width);
  data_write(camera,MT9M111_WINDOW_HEIGHT, rect->height);

	if (code != MBUS_FMT_SBGGR10_2X8_PADHI_LE) {
	 setup_rect(camera, &context_b,
						     rect, width, height);
		setup_rect(camera, &context_a,
						     rect, width, height);
	}
	return CAM_OK;
}
uint8_t soft_enable(uint8_t camera)
{
	data_write(camera,MT9M111_RESET, MT9M111_RESET_CHIP_ENABLE);
	return CAM_OK;
}
uint8_t soft_reset(uint8_t camera)
{
	mt_reg_set(camera, MT9M111_RESET, MT9M111_RESET_RESET_MODE);
	mt_reg_set(camera, RESET, MT9M111_RESET_RESET_SOC);
	mt_reg_clear(camera, RESET, MT9M111_RESET_RESET_MODE | MT9M111_RESET_RESET_SOC);

	return CAM_OK;
}

uint8_t reg_set(uint8_t camera,uint16_t reg,uint16_t data)
{
	uint8_t ret[2];
	reg_read(camera,reg & 0xff,ret);
	uint16_t ret16 = ret[1] + (ret[0] <<	8);
	data_write(camera,reg & 0xff, ret16 | data);
	return CAM_OK;
}
uint8_t reg_clear(uint8_t camera,uint16_t reg,uint16_t data)
{
	uint8_t ret[2];
	reg_read(camera,reg & 0xff,ret);
	uint16_t ret16 = ret[1] + (ret[0] <<	8);
	data_write(camera,reg & 0xff, ret16 & ~data);
	return CAM_OK;
}
uint8_t mt_reg_mask(uint8_t camera,uint16_t reg,uint16_t mask , uint16_t data)
{
	uint8_t ret[2];
	reg_read(camera,reg & 0xff,ret);
	uint16_t ret16 = ret[1] + (ret[0] <<	8);
	data_write(camera,reg & 0xff, (ret16 & ~mask) | data);
	return CAM_OK;
}
int set_context(uint8_t camera,struct mt9m111_context *ctx)
{
  data_write(camera,MT9M111_CONTEXT_CONTROL, ctx->control);
	return CAM_OK;
}
uint8_t set_reg_page(uint8_t camera,uint16_t reg)
{
	if(camera == MT1ADDR)
	{
		CAM1_EN();
	}
	else if(camera == MT2ADDR)
	{
		CAM2_EN();
	}
	data_write(camera,MT9M111_PAGE_MAP,(uint16_t)(reg>>8));
	return CAM_OK;
}

uint8_t *prepend_array(uint16_t reg,uint16_t data)
{
	if(data == NULL)
	{
		static uint8_t *ret;
		ret = (uint8_t *)malloc(1);
		ret[0] = reg & 0xFF;
		return ret;
	}
	static uint8_t ret[3];
	ret[0] = reg & 0xff;
	ret[1] = data >> 8;
	ret[2] = data & 0xff;
	return ret;
}

uint32_t GetSize(uint8_t reso)
{
	uint32_t size = 0;
	switch(reso)
	{
		case RESO_1280x1024:		//RESO_SXGA
			size = 0xA0000;
			break;
		case RESO_640x512:			//RESO_QSXGA
			size = 0x28000;
			break;
		case RESO_640x480:			//VGA
			size = 0x25800;
			break;
		case RESO_320x240:			//CIF also known as QVGA
			size = 0x9600;
			break;
		case RESO_160x120:			//QCIF
			size = 0x2580;
			break;
		default:
			break;
	}
	return size;
}


static int mt9m111_set_pixfmt(uint8_t camera, enum mbus_pixelcode code)
{

	u16 data_outfmt2, mask_outfmt2 = 	MT9M111_OUTFMT_PROCESSED_BAYER |
																		MT9M111_OUTFMT_BYPASS_IFP | MT9M111_OUTFMT_RGB |
																		MT9M111_OUTFMT_RGB565 | MT9M111_OUTFMT_RGB555 |
																		MT9M111_OUTFMT_RGB444x | MT9M111_OUTFMT_RGBx444 |
																		MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN |
																		MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr_RGB_R_B;

	switch (code) {
	case MBUS_FMT_SBGGR8_1X8:
		data_outfmt2 = MT9M111_OUTFMT_PROCESSED_BAYER |
			MT9M111_OUTFMT_RGB;
		break;
	case MBUS_FMT_SBGGR10_2X8_PADHI_LE:
		data_outfmt2 = MT9M111_OUTFMT_BYPASS_IFP | MT9M111_OUTFMT_RGB;
		break;
	case MBUS_FMT_RGB555_2X8_PADHI_LE:
		data_outfmt2 = MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB555 |
			MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN;
		break;
	case MBUS_FMT_RGB555_2X8_PADHI_BE:
		data_outfmt2 = MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB555;
		break;
	case MBUS_FMT_RGB565_2X8_LE:
		data_outfmt2 = MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB565 |
			MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN;
		break;
	case MBUS_FMT_RGB565_2X8_BE:
		data_outfmt2 = MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB565;
		break;
	case MBUS_FMT_BGR565_2X8_BE:
		data_outfmt2 = MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB565 |
			MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr_RGB_R_B;
		break;
	case MBUS_FMT_BGR565_2X8_LE:
		data_outfmt2 = MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB565 |
			MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN |
			MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr_RGB_R_B;
		break;
	case MBUS_FMT_UYVY8_2X8:
		data_outfmt2 = 0;
		break;
	case MBUS_FMT_VYUY8_2X8:
		data_outfmt2 = MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr_RGB_R_B;
		break;
	case MBUS_FMT_YUYV8_2X8:
		data_outfmt2 = MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN;
		break;
	case MBUS_FMT_YVYU8_2X8:
		data_outfmt2 = MT9M111_OUTFMT_SWAP_YCbCr_C_Y_RGB_EVEN |
			MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr_RGB_R_B;
		break;
	default:
		return CAM_ERR;
	}
	mt9m111_reg_mask(camera, context_a.output_fmt_ctrl2,
			       data_outfmt2, mask_outfmt2);
	mt9m111_reg_mask(camera, context_b.output_fmt_ctrl2,
			       data_outfmt2, mask_outfmt2);

	return CAM_OK;
}

static int mt9m111_try_fmt(struct rect *rect,struct mbus_framefmt *mf)
{
	const struct mt9m111_datafmt *fmt;
	uint8_t bayer;

	fmt = find_datafmt( mf->code);

	bayer = fmt->code == MBUS_FMT_SBGGR8_1X8 ||
		fmt->code == MBUS_FMT_SBGGR10_2X8_PADHI_LE;

	/*
	 * With Bayer format enforce even side lengths, but let the user play
	 * with the starting pixel
	 */
	if (bayer) {
		rect->width = ALIGN(rect->width, 2);
		rect->height = ALIGN(rect->height, 2);
	}

	if (fmt->code == MBUS_FMT_SBGGR10_2X8_PADHI_LE) {
		/* IFP bypass mode, no scaling */
		mf->width = rect->width;
		mf->height = rect->height;
	} else {
		/* No upscaling */
		if (mf->width > rect->width)
			mf->width = rect->width;
		if (mf->height > rect->height)
			mf->height = rect->height;
	}
	char msg[30];
	sprintf(msg,"%s(): %ux%u, code=%x\n", __func__,
		mf->width, mf->height, fmt->code);
	Debug(msg);
	
	mf->code = fmt->code;
	mf->colorspace = fmt->colorspace;

	return 0;
}

static int mt9m111_s_fmt(uint8_t camera, struct rect *rect, struct mbus_framefmt *mf)
{
	const struct mt9m111_datafmt *fmt;
	int ret;

	mt9m111_try_fmt(rect, mf);
	fmt = find_datafmt(mf->code);
	/* try_fmt() guarantees fmt != NULL && fmt->code == mf->code */

	ret = mt9m111_setup_geometry(camera, rect, mf->width, mf->height, mf->code);
	if (!ret)
		ret = mt9m111_set_pixfmt(camera, mf->code);
//	if (!ret) {
//		mt9m111->width	= mf->width;
//		mt9m111->height	= mf->height;
//		mt9m111->fmt	= fmt;
//	}

	return ret;
}
 

static int mt9m111_set_flip(uint8_t camera, int flip, int mask)
{
	int ret;

	if (flip)
		ret = reg_set(camera, mt9m111->ctx->read_mode, mask);
	else
		ret = reg_clear(camera, mt9m111->ctx->read_mode, mask);

	return ret;
}

static int mt9m111_get_global_gain(struct mt9m111 *mt9m111)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);
	int data;

	data = reg_read(GLOBAL_GAIN);
	if (data >= 0)
		return (data & 0x2f) * (1 << ((data >> 10) & 1)) *
			(1 << ((data >> 9) & 1));
	return data;
}

static int mt9m111_set_global_gain(struct mt9m111 *mt9m111, int gain)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);
	u16 val;

	if (gain > 63 * 2 * 2)
		return -EINVAL;

	if ((gain >= 64 * 2) && (gain < 63 * 2 * 2))
		val = (1 << 10) | (1 << 9) | (gain / 4);
	else if ((gain >= 64) && (gain < 64 * 2))
		val = (1 << 9) | (gain / 2);
	else
		val = gain;

	return reg_write(GLOBAL_GAIN, val);
}

static int mt9m111_set_autoexposure(struct mt9m111 *mt9m111, int val)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);

	if (val == EXPOSURE_AUTO)
		return reg_set(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOEXPO_EN);
	return reg_clear(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOEXPO_EN);
}

static int mt9m111_set_autowhitebalance(struct mt9m111 *mt9m111, int on)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);

	if (on)
		return reg_set(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOWHITEBAL_EN);
	return reg_clear(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOWHITEBAL_EN);
}

static int mt9m111_s_ctrl(struct ctrl *ctrl)
{
	struct mt9m111 *mt9m111 = container_of(ctrl->handler,
					       struct mt9m111, hdl);

	switch (ctrl->id) {
	case CID_VFLIP:
		return mt9m111_set_flip(mt9m111, ctrl->val,
					MT9M111_RMB_MIRROR_ROWS);
	case CID_HFLIP:
		return mt9m111_set_flip(mt9m111, ctrl->val,
					MT9M111_RMB_MIRROR_COLS);
	case CID_GAIN:
		return mt9m111_set_global_gain(mt9m111, ctrl->val);
	case CID_EXPOSURE_AUTO:
		return mt9m111_set_autoexposure(mt9m111, ctrl->val);
	case CID_AUTO_WHITE_BALANCE:
		return mt9m111_set_autowhitebalance(mt9m111, ctrl->val);
	}

	return -EINVAL;
}

static int mt9m111_suspend(struct mt9m111 *mt9m111)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);
	int ret;

	ctrl_s_ctrl(mt9m111->gain, mt9m111_get_global_gain(mt9m111));

	ret = reg_set(RESET, MT9M111_RESET_RESET_MODE);
	if (!ret)
		ret = reg_set(RESET, MT9M111_RESET_RESET_SOC |
			      MT9M111_RESET_OUTPUT_DISABLE |
			      MT9M111_RESET_ANALOG_STANDBY);
	if (!ret)
		ret = reg_clear(RESET, MT9M111_RESET_CHIP_ENABLE);

	return ret;
}

static void mt9m111_restore_state(struct mt9m111 *mt9m111)
{
	mt9m111_set_context(mt9m111, mt9m111->ctx);
	mt9m111_set_pixfmt(mt9m111, mt9m111->fmt->code);
	mt9m111_setup_geometry(mt9m111, &mt9m111->rect,
			mt9m111->width, mt9m111->height, mt9m111->fmt->code);
	ctrl_handler_setup(&mt9m111->hdl);
}

static int mt9m111_resume(struct mt9m111 *mt9m111)
{
	int ret = mt9m111_enable(mt9m111);
	if (!ret)
		ret = mt9m111_reset(mt9m111);
	if (!ret)
		mt9m111_restore_state(mt9m111);

	return ret;
}

static int mt9m111_init(struct mt9m111 *mt9m111)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);
	int ret;

	ret = mt9m111_enable(mt9m111);
	if (!ret)
		ret = mt9m111_reset(mt9m111);
	if (!ret)
		ret = mt9m111_set_context(mt9m111, mt9m111->ctx);
	if (ret)
		dev_err(&client->dev, "mt9m111 init failed: %d\n", ret);
	return ret;
}

static int mt9m111_power_on(struct mt9m111 *mt9m111)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);
	struct soc_camera_subdev_desc *ssdd = soc_camera_i2c_to_desc(client);
	int ret;

	ret = soc_camera_power_on(&client->dev, ssdd, mt9m111->clk);
	if (ret < 0)
		return ret;

	ret = mt9m111_resume(mt9m111);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to resume the sensor: %d\n", ret);
		soc_camera_power_off(&client->dev, ssdd, mt9m111->clk);
	}

	return ret;
}

static void mt9m111_power_off(struct mt9m111 *mt9m111)
{
	struct i2c_client *client = get_subdevdata(&mt9m111->subdev);
	struct soc_camera_subdev_desc *ssdd = soc_camera_i2c_to_desc(client);

	mt9m111_suspend(mt9m111);
	soc_camera_power_off(&client->dev, ssdd, mt9m111->clk);
}

static int mt9m111_s_power(struct subdev *sd, int on)
{
	struct mt9m111 *mt9m111 = container_of(sd, struct mt9m111, subdev);
	int ret = 0;

	mutex_lock(&mt9m111->power_lock);

	/*
	 * If the power count is modified from 0 to != 0 or from != 0 to 0,
	 * update the power state.
	 */
	if (mt9m111->power_count == !on) {
		if (on)
			ret = mt9m111_power_on(mt9m111);
		else
			mt9m111_power_off(mt9m111);
	}

	if (!ret) {
		/* Update the power count. */
		mt9m111->power_count += on ? 1 : -1;
		WARN_ON(mt9m111->power_count < 0);
	}

	mutex_unlock(&mt9m111->power_lock);
	return ret;
}

