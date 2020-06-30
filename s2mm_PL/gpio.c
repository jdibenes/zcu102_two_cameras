/*
 * gpio.c
 *
 *  Created on: Apr 1, 2018
 *      Author: jcds
 */

#include "xgpio.h"

int gpio_init(XGpio *p_gpio, u16 dev_id) {
	XGpio_Config *cfg;
	int status;

	cfg = XGpio_LookupConfig(dev_id);
	if (cfg == NULL) { return -1; }

	status = XGpio_CfgInitialize(p_gpio, cfg, cfg->BaseAddress);
	if (status != XST_SUCCESS) { return -2; }

	XGpio_SetDataDirection(p_gpio, 1, 0xFFFFFFFF);

	return 0;
}

u32 gpio_read_capture(XGpio *p_gpio) {
	u32 data;

	data = XGpio_DiscreteRead(p_gpio, 1);
	return data & 0x1F;
}
