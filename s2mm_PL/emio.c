/*
 * emio.c
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#include "xgpiops.h"

#define GPIO_BANK3_PIN0 78

#define EMIO_PIN_SENSOR_RESET   (GPIO_BANK3_PIN0 + 12)
#define EMIO_PIN_FRMBUFWR_RESET (GPIO_BANK3_PIN0 +  2)
#define EMIO_PIN_SSSCALER_RESET (GPIO_BANK3_PIN0 +  4)
#define EMIO_PIN_SSCSC_RESET    (GPIO_BANK3_PIN0 +  6)
#define EMIO_PIN_GAMMALUT_RESET (GPIO_BANK3_PIN0 +  8)
#define EMIO_PIN_DEMOSAIC_RESET (GPIO_BANK3_PIN0 +  7)

static void emio_cfg_output(XGpioPs *p_emio, u32 pin) {
	XGpioPs_SetDirectionPin(p_emio, pin, 1);
	XGpioPs_SetOutputEnablePin(p_emio, pin, 1);
}

int emio_init(XGpioPs *p_emio) {
	XGpioPs_Config *cfg;

	cfg = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	if (cfg == NULL) { return -1; }

	XGpioPs_CfgInitialize(p_emio, cfg, cfg->BaseAddr);

	emio_cfg_output(p_emio, EMIO_PIN_SENSOR_RESET);
	emio_cfg_output(p_emio, EMIO_PIN_FRMBUFWR_RESET);
	emio_cfg_output(p_emio, EMIO_PIN_SSSCALER_RESET);
	emio_cfg_output(p_emio, EMIO_PIN_SSCSC_RESET);
	emio_cfg_output(p_emio, EMIO_PIN_GAMMALUT_RESET);
	emio_cfg_output(p_emio, EMIO_PIN_DEMOSAIC_RESET);

	return 0;
}

void emio_set_sensor_reset  (XGpioPs *p_emio, u32 bit) { XGpioPs_WritePin(p_emio, EMIO_PIN_SENSOR_RESET,   bit); }
void emio_set_frmbufwr_reset(XGpioPs *p_emio, u32 bit) { XGpioPs_WritePin(p_emio, EMIO_PIN_FRMBUFWR_RESET, bit); }
void emio_set_ssscaler_reset(XGpioPs *p_emio, u32 bit) { XGpioPs_WritePin(p_emio, EMIO_PIN_SSSCALER_RESET, bit); }
void emio_set_sscsc_reset   (XGpioPs *p_emio, u32 bit) { XGpioPs_WritePin(p_emio, EMIO_PIN_SSCSC_RESET,    bit); }
void emio_set_gammalut_reset(XGpioPs *p_emio, u32 bit) { XGpioPs_WritePin(p_emio, EMIO_PIN_GAMMALUT_RESET, bit); }
void emio_set_demosaic_reset(XGpioPs *p_emio, u32 bit) { XGpioPs_WritePin(p_emio, EMIO_PIN_DEMOSAIC_RESET, bit); }

