/*
 * gammalut.h
 *
 *  Created on: Feb 20, 2018
 *      Author: jcds
 */

#ifndef SRC_GAMMALUT_H_
#define SRC_GAMMALUT_H_

#include "xv_gamma_lut.h"
#include "xgpiops.h"

int gammalut_init(XV_gamma_lut *p_gammalut, u16 dev_id);
void gammalut_setup(XV_gamma_lut *p_gammalut, u32 width, u32 height, u32 format);
void gammalut_start(XV_gamma_lut *p_gammalut);
void dual_gammalut_reset(XGpioPs *p_emio);
void gammalut_set_threshold(XV_gamma_lut *p_gammalut, u32 r, u32 g, u32 b);

#endif /* SRC_GAMMALUT_H_ */
