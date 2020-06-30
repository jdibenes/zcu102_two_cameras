/*
 * emio.h
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#ifndef SRC_EMIO_H_
#define SRC_EMIO_H_

#include "xgpiops.h"

#define EMIO_RESET_DELAY_US 10000

int emio_init(XGpioPs *p_emio);

void emio_set_sensor_reset  (XGpioPs *p_emio, u32 bit);
void emio_set_frmbufwr_reset(XGpioPs *p_emio, u32 bit);
void emio_set_ssscaler_reset(XGpioPs *p_emio, u32 bit);
void emio_set_sscsc_reset   (XGpioPs *p_emio, u32 bit);
void emio_set_gammalut_reset(XGpioPs *p_emio, u32 bit);
void emio_set_demosaic_reset(XGpioPs *p_emio, u32 bit);
void emio_set_dslp3p_reset  (XGpioPs *p_emio, u32 bit);

#endif /* SRC_EMIO_H_ */
