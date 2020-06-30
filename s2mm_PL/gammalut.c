/*
 * gammalut.c
 *
 *  Created on: Feb 20, 2018
 *      Author: jcds
 */

#include "xv_gamma_lut.h"
#include "emio.h"
#include "sleep.h"

static void gammalut_set_passthrough(XV_gamma_lut *p_gammalut) {
	int n;
	int data;

	for (n = 0; n < 512; ++n) {
		data = (((2 * n) + 1) << 16) | (2 * n);
		XV_gamma_lut_Write_HwReg_gamma_lut_0_Words(p_gammalut, n, &data, 1);
		XV_gamma_lut_Write_HwReg_gamma_lut_1_Words(p_gammalut, n, &data, 1);
		XV_gamma_lut_Write_HwReg_gamma_lut_2_Words(p_gammalut, n, &data, 1);
	}
}

void gammalut_set_threshold(XV_gamma_lut *p_gammalut, u32 r, u32 g, u32 b) {
	int data_r, data_g, data_b;
	int n;
	int n0;
	int n1;
	int n0_r, n0_g, n0_b, n1_r, n1_g, n1_b;

	for (n = 0; n < 512; ++n) {
		n0 = (2 * n);
		n1 = (2 * n) + 1;

		n0_r = n0 < r ? 0 : 0xFF;
		n1_r = n1 < r ? 0 : 0xFF;

		n0_g = n0 < g ? 0 : 0xFF;
		n1_g = n1 < g ? 0 : 0xFF;

		n0_b = n0 < b ? 0 : 0xFF;
		n1_b = n1 < b ? 0 : 0xFF;

		data_r = (n1_r << 16) | n0_r;
		data_g = (n1_g << 16) | n0_g;
		data_b = (n1_b << 16) | n0_b;

		XV_gamma_lut_Write_HwReg_gamma_lut_0_Words(p_gammalut, n, &data_r, 1);
		XV_gamma_lut_Write_HwReg_gamma_lut_1_Words(p_gammalut, n, &data_g, 1);
		XV_gamma_lut_Write_HwReg_gamma_lut_2_Words(p_gammalut, n, &data_b, 1);
	}
}

int gammalut_init(XV_gamma_lut *p_gammalut, u16 dev_id) {
	XV_gamma_lut_Config *cfg;
	int status;

	cfg = XV_gamma_lut_LookupConfig(dev_id);
	if (cfg == NULL) { return -1; }

	status = XV_gamma_lut_CfgInitialize(p_gammalut, cfg, cfg->BaseAddress);
	if (status != XST_SUCCESS) { return -2; }

	XV_gamma_lut_InterruptGlobalDisable(p_gammalut);
	return 0;
}

void gammalut_setup(XV_gamma_lut *p_gammalut, u32 width, u32 height, u32 format) {
	XV_gamma_lut_Set_HwReg_width(p_gammalut, width);
	XV_gamma_lut_Set_HwReg_height(p_gammalut, height);
	XV_gamma_lut_Set_HwReg_video_format(p_gammalut, format);

	gammalut_set_passthrough(p_gammalut);
}

void gammalut_start(XV_gamma_lut *p_gammalut) {
	XV_gamma_lut_EnableAutoRestart(p_gammalut);
	XV_gamma_lut_Start(p_gammalut);
}

void dual_gammalut_reset(XGpioPs *p_emio) {
	emio_set_gammalut_reset(p_emio, 0);
	usleep(EMIO_RESET_DELAY_US);
	emio_set_gammalut_reset(p_emio, 1);
	usleep(EMIO_RESET_DELAY_US);
}
