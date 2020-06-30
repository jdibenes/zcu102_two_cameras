/*
 * demosaic.c
 *
 *  Created on: Feb 20, 2018
 *      Author: jcds
 */

#include "xv_demosaic.h"
#include "xgpiops.h"
#include "emio.h"
#include "sleep.h"

int demosaic_init(XV_demosaic *p_demosaic, u16 dev_id) {
	XV_demosaic_Config *cfg;
	int status;

	cfg = XV_demosaic_LookupConfig(dev_id);
	if (cfg == NULL) { return -1; }

	status = XV_demosaic_CfgInitialize(p_demosaic, cfg, cfg->BaseAddress);
	if (status != XST_SUCCESS) { return -2; }

	XV_demosaic_InterruptGlobalDisable(p_demosaic);
	return 0;
}

void demosaic_setup(XV_demosaic *p_demosaic, u32 width, u32 height, u32 phase) {
	XV_demosaic_Set_HwReg_width(p_demosaic, width);
	XV_demosaic_Set_HwReg_height(p_demosaic, height);
	XV_demosaic_Set_HwReg_bayer_phase(p_demosaic, phase);
}

void demosaic_start(XV_demosaic *p_demosaic) {
	XV_demosaic_EnableAutoRestart(p_demosaic);
	XV_demosaic_Start(p_demosaic);
}

void dual_demosaic_reset(XGpioPs *p_emio) {
	emio_set_demosaic_reset(p_emio, 0);
	usleep(EMIO_RESET_DELAY_US);
	emio_set_demosaic_reset(p_emio, 1);
	usleep(EMIO_RESET_DELAY_US);
}
