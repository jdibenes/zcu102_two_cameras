/*
 * demosaic.h
 *
 *  Created on: Feb 20, 2018
 *      Author: jcds
 */

#ifndef SRC_DEMOSAIC_H_
#define SRC_DEMOSAIC_H_

#include "xv_demosaic.h"
#include "xgpiops.h"

int demosaic_init(XV_demosaic *p_demosaic, u16 dev_id);
void demosaic_setup(XV_demosaic *p_demosaic, u32 width, u32 height, u32 phase);
void demosaic_start(XV_demosaic *p_demosaic);
void dual_demosaic_reset(XGpioPs *p_emio);

#endif /* SRC_DEMOSAIC_H_ */
