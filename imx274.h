/*
 * sensor_i2c.h
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#ifndef SRC_IMX274_H_
#define SRC_IMX274_H_

#include "s2mm_PL/emio.h"
#include "s2mm_PL/sensor_i2c.h"

struct v4l2_fract {
	u32 numerator;
	u32 denominator;
};

/*
 * struct stim274 - imx274 device structure
 * @sd: V4L2 subdevice structure
 * @pd: Media pad structure
 * @client: Pointer to I2C client
 * @ctrls: imx274 control structure
 * @format: V4L2 media bus frame format structure
 * @frame_rate: V4L2 frame rate structure
 * @regmap: Pointer to regmap structure
 * @reset_gpio: Pointer to reset gpio
 * @lock: Mutex structure
 * @mode_index: Resolution mode index
 */
struct stimx274 {
	//struct v4l2_subdev sd;
	//struct media_pad pad;
	//struct i2c_client *client;
	//struct imx274_ctrls ctrls;
	//struct v4l2_mbus_framefmt format;
	struct v4l2_fract frame_interval;
	//struct regmap *regmap;
	//struct gpio_desc *reset_gpio;
	//struct mutex lock; /* mutex lock for operations */
	XIicEx iic_ex;
	u32 mode_index;
};

void dual_imx274_reset(XGpioPs *p_emio, int rst);

int imx274_mode_regs(struct stimx274 *priv, int mode);
int imx274_start_stream(struct stimx274 *priv);


int imx274_s_stream(struct stimx274 *imx274, int on);

int imx274_set_frame_interval(struct stimx274 *priv, struct v4l2_fract frame_interval);
int imx274_set_test_pattern(struct stimx274 *priv, int val);
int imx274_set_exposure(struct stimx274 *priv, int val);
int imx274_set_vflip(struct stimx274 *priv, int val);
int imx274_set_gain(struct stimx274 *priv, u32 val);

#endif /* SRC_IMX274_H_ */
