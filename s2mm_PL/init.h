/*
 * init.h
 *
 *  Created on: Feb 2, 2021
 *      Author: jcds
 */

#ifndef SRC_S2MM_PL_INIT_H_
#define SRC_S2MM_PL_INIT_H_

#include "irq.h"
#include "emio.h"
#include "gpio.h"
#include "sensor_i2c.h"

#include "csi_rx.h"
#include "demosaic.h"
#include "gammalut.h"
#include "procss.h"
#include "frmbufwr.h"

int common_init(XIntc *p_intc, XScuGic *p_apugic, XGpioPs *p_emio, XGpio *p_gpio);
int dual_iic_init(XIicEx *p_iic_ex_0, XIicEx *p_iic_ex_1, XIntc *p_intc);

int dual_csi_init(XCsiSs *p_csi_0, XCsiSs *p_csi_1, XIntc *p_intc);
int dual_demosaic_init(XV_demosaic *p_demosaic_0, XV_demosaic *p_demosaic_1, XGpioPs *p_emio, u32 width, u32 height);
int dual_gammalut_init(XV_gamma_lut *p_gammalut_0, XV_gamma_lut *p_gammalut_1, XGpioPs *p_emio, u32 width, u32 height);
int dual_procss_init(XVprocSs *p_csc_0, XVprocSs *p_scaler_0, XVprocSs *p_csc_1, XVprocSs *p_scaler_1, XGpioPs *p_emio, u32 width, u32 height, u32 frame_rate);
int dual_frmbufwr_init(XV_FrmbufWr_l2 *p_frmbufwr_0, XV_FrmbufWr_l2 *p_frmbufwr_1, XGpioPs *p_emio, XIntc *p_intc, u32 width, u32 height, u32 frame_rate);

#endif /* SRC_S2MM_PL_INIT_H_ */
