/*
 * procss.h
 *
 *  Created on: Feb 21, 2018
 *      Author: jcds
 */

#ifndef SRC_PROCSS_H_
#define SRC_PROCSS_H_

#include "xvprocss.h"
#include "xgpiops.h"

int procss_init(XVprocSs *p_csc, u16 dev_id_csc, XVprocSs *p_scaler, u16 dev_id_scaler);
int procss_setup(XVprocSs *p_csc, XVprocSs *p_scaler, u32 width, u32 height, u32 frame_rate);
void procss_csc(XVprocSs *p_csc, s32 brightness, s32 contrast, s32 saturation, s32 rgain, s32 ggain, s32 bgain);
int procss_start(XVprocSs *p_csc, XVprocSs *p_scaler);
void dual_procss_reset(XGpioPs *p_emio);

#endif /* SRC_S2MM_PL_PROCSS_H_ */
