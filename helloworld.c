/*
 * helloworld.c
 *
 *  Created on: Feb 2, 2021
 *      Author: jcds
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"

#include "s2mm_PL/init.h"
#include "capture.h"

int main() {
	u32 mode       = 0;
	u32 width      = 3840;
	u32 height     = 2160;
	u32 frame_rate = 60;
	u32 bpp        = 2;
	u32 size       = width * height * bpp;
	int frameid    = 0;
	u32 gpiobits   = 0;

	FATFS fatfs;

	XIntc intc;
	XScuGic apugic;
	XGpioPs emio;
	XGpio gpio;

	XV_FrmbufWr_l2 frmbufwr_0, frmbufwr_1;
	XVprocSs csc_0, scaler_0, csc_1, scaler_1;
	XV_gamma_lut gammalut_0, gammalut_1;
	XV_demosaic demosaic_0, demosaic_1;
	XCsiSs csi_0, csi_1;
	struct stimx274 imx274_0, imx274_1;

	char fname0[16];
	char fname1[16];
	int status;

	// Initialize -------------------------------------------------------------
    init_platform();

    print("Hello World\n\r");

    xil_printf("init SD\n\r");
	status = init_sd(&fatfs);
	if (status != XST_SUCCESS) {
		xil_printf("init SD failed\n\r");
		return 0;
	}

    status = common_init(&intc, &apugic, &emio, &gpio);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_iic_init(&imx274_0.iic_ex, &imx274_1.iic_ex, &intc);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_frmbufwr_init(&frmbufwr_0, &frmbufwr_1, &emio, &intc, width, height, frame_rate);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_procss_init(&csc_0, &scaler_0, &csc_1, &scaler_1, &emio, width, height, frame_rate);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_gammalut_init(&gammalut_0, &gammalut_1, &emio, width, height);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_demosaic_init(&demosaic_0, &demosaic_1, &emio, width, height);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_csi_init(&csi_0, &csi_1, &intc);
    if (status != XST_SUCCESS) { return 0; }

    xil_printf("intc_start\n\r");
	status = intc_start(&intc);
	if (status != 0) {
		xil_printf("intc_start failed! (%d)\n\r", status);
		return 0;
	}

	frmbufwr_start(&frmbufwr_0);
	frmbufwr_start(&frmbufwr_1);

	status = procss_start(&csc_0, &scaler_0);
	if (status != 0) {
		xil_printf("procss_start 0 failed! (%d)\n\r", status);
		return 0;
	}

	status = procss_start(&csc_1, &scaler_1);
	if (status != 0) {
		xil_printf("procss_start 1 failed! (%d)\n\r", status);
		return 0;
	}

	gammalut_start(&gammalut_0);
	gammalut_start(&gammalut_1);

	demosaic_start(&demosaic_0);
	demosaic_start(&demosaic_1);

	xil_printf("imx274 reset\n\r");
	dual_imx274_reset(&emio, 1);

	xil_printf("imx274 init\n\r");
	sensor_init(&imx274_0, mode, frame_rate, 16636 * 1,   5632);
	sensor_init(&imx274_1, mode, frame_rate, 16636 * 0.5, 5632);

	xil_printf("csi_activate 0\n\r");
	status = csi_start(&csi_0);
	if (status != XST_SUCCESS) {
		xil_printf("csi_activate 0 failed!\n\r");
		return 0;
	}

	xil_printf("csi_activate 1\n\r");
	status = csi_start(&csi_1);
	if (status != XST_SUCCESS) {
		xil_printf("csi_activate 1 failed!\n\r");
		return 0;
	}

	// Main loop --------------------------------------------------------------
	xil_printf("Ready to capture!\n\r");
	xil_printf("Press any of SW14, SW15, SW16, SW17, or SW18 to take a picture.\n\r");

	for (;;) {
		gpiobits = (gpiobits << 5) | gpio_read_capture(&gpio);

		if ((~(gpiobits >> 5) & gpiobits) & 0x1F) {
			xil_printf("Capturing...\n\r");

			sprintf(fname0, "0:/i0f%d.raw", frameid);
			sprintf(fname1, "0:/i1f%d.raw", frameid);

			frameid = (frameid + 1) & 0x7FFF;

			capture_images(&frmbufwr_0, &frmbufwr_1, fname0, fname1, size);

			sleep(1);
			gpiobits = 0;
		}
		else {
			usleep(17*1000);
		}
	}

    cleanup_platform();
    return 0;
}
