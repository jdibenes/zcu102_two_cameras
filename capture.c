/*
 * capture.c
 *
 *  Created on: Feb 2, 2021
 *      Author: jcds
 */

#include "xil_cache.h"
#include "capture.h"

int init_sd(FATFS *p_fatfs) {
	FRESULT res;

	res = f_mount(p_fatfs, "0:/", 0);
	if (res != FR_OK) { return XST_FAILURE; }
	return XST_SUCCESS;
}

int save_file_sd(char *filename, UINTPTR src_addr, int size) {
	FIL fil;
	UINT byteswritten;
	FRESULT res;

	res = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (res) { return -1; }

	res = f_lseek(&fil, 0);
	if (res) { return -2; }

	res = f_write(&fil, (void *)src_addr, size, &byteswritten);
	if (res) { return -3; }

	res = f_close(&fil);
	if (res) { return -4; }

	return 0;
}

void capture_images(XV_FrmbufWr_l2 *p_frmbufwr_0, XV_FrmbufWr_l2 *p_frmbufwr_1, char *fname0, char *fname1, u32 size) {
	int status;

	frmbufwr_lock();

	Xil_DCacheFlush();
	Xil_DCacheInvalidate();

	status = save_file_sd(fname0, (UINTPTR)frmbufwr_getaddr(p_frmbufwr_0), size);
	if (status == 0) {
		xil_printf("Wrote %s\n\r", fname0);
	}
	else {
		xil_printf("Failed to write %s (%d)\n\r", fname0, status);
	}

	status = save_file_sd(fname1, (UINTPTR)frmbufwr_getaddr(p_frmbufwr_1), size);
	if (status == 0) {
		xil_printf("Wrote %s\n\r", fname1);
	}
	else {
		xil_printf("Failed to write %s (%d)\n\r", fname1, status);
	}

	frmbufwr_release();
}

void sensor_init(struct stimx274 *p_imx274, u32 mode, u32 frame_rate, u32 exposure, u32 gain) {
	p_imx274->mode_index = mode;
	p_imx274->frame_interval.numerator = 1;
	p_imx274->frame_interval.denominator = frame_rate;

	xil_printf("imx264 set tp (%d)\n\r", imx274_set_test_pattern(p_imx274, 0));
	xil_printf("imx264 set vf (%d)\n\r", imx274_set_vflip(p_imx274, 0));
	xil_printf("imx264 set gain (%d)\n\r", imx274_set_gain(p_imx274, gain));

	xil_printf("imx264 mode (%d)\n\r", imx274_mode_regs(p_imx274, 0));
	xil_printf("imx264 set interval (%d)\n\r", imx274_set_frame_interval(p_imx274, p_imx274->frame_interval));
	xil_printf("imx264 set exp (%d)\n\r", imx274_set_exposure(p_imx274, exposure));
	xil_printf("imx264 start (%d)\n\r", imx274_start_stream(p_imx274));
}
