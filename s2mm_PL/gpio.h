/*
 * gpio.h
 *
 *  Created on: Apr 1, 2018
 *      Author: jcds
 */

#ifndef SRC_S2MM_PL_GPIO_H_
#define SRC_S2MM_PL_GPIO_H_

#include "xgpio.h"

int gpio_init(XGpio *p_gpio, u16 dev_id) ;
u32 gpio_read_capture(XGpio *p_gpio);

#endif /* SRC_S2MM_PL_GPIO_H_ */
