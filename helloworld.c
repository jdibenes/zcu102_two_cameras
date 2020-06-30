/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "imx274.h"

#include "sleep.h"
#include "ff.h"
#include "xil_cache.h"

#include "s2mm_PL/demosaic.h"
#include "s2mm_PL/gammalut.h"
#include "s2mm_PL/gpio.h"
#include "s2mm_PL/irq.h"
#include "s2mm_PL/sensor_i2c.h"
#include "s2mm_PL/procss.h"
#include "s2mm_PL/frmbufwr.h"
#include "s2mm_PL/csi_rx.h"

extern u32 g_lock;
extern u32 g_target_0;
extern u32 g_target_1;

#define IMX274_IIC_ADDR 0x1A

int init_sd(FATFS *p_fatfs) {
	FRESULT res;

	res = f_mount(p_fatfs, "0:/", 0);
	if (res != FR_OK) { return XST_FAILURE; }
	return XST_SUCCESS;
}

int save_pic_sd(char *filename, UINTPTR src_addr, int size) {
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

//

int common_init(XIntc *p_intc, XScuGic *p_apugic, XGpioPs *p_emio) {
	int status;

	xil_printf("intc_init\n\r");
	status = intc_init(p_intc, p_apugic);
	if (status != 0) {
		xil_printf("intc_init failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("emio_init\n\r");
	status = emio_init(p_emio);
	if (status != 0) {
		xil_printf("init_shared_emio failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int dual_iic_init(XIicEx *p_iic_ex_0, XIicEx *p_iic_ex_1, XIntc *p_intc) {
	int status;

	xil_printf("iic_init 0\n\r");
	status = iic_init(p_iic_ex_0, XPAR_IIC_0_DEVICE_ID, p_intc, XPAR_INTC_0_IIC_0_VEC_ID, IMX274_IIC_ADDR);
	if (status != 0) {
		xil_printf("iic_init 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("iic_init 1\n\r");
	status = iic_init(p_iic_ex_1, XPAR_IIC_1_DEVICE_ID, p_intc, XPAR_INTC_0_IIC_1_VEC_ID, IMX274_IIC_ADDR);
	if (status != 0) {
		xil_printf("iic_init 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int dual_frmbufwr_init(XV_FrmbufWr_l2 *p_frmbufwr_0, XV_FrmbufWr_l2 *p_frmbufwr_1, XGpioPs *p_emio, XIntc *p_intc, u32 width, u32 height, u32 frame_rate, UINTPTR addr_0, UINTPTR addr_1) {
	int status;

	xil_printf("dual_frmbufwr_reset\n\r");
	dual_frmbufwr_reset(p_emio);

	xil_printf("frmbufwr_init 0\n\r");
	status = frmbufwr_init(p_frmbufwr_0, XPAR_MIPI_CSI2_RX0_V_FRMBUF_WR_0_DEVICE_ID, p_intc, XPAR_INTC_0_V_FRMBUF_WR_0_VEC_ID);
	if (status != 0) {
		xil_printf("frmbufwr_init 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("frmbufwr_setup 0\n\r");
	status = frmbufwr_setup(p_frmbufwr_0, width, height, frame_rate, addr_0);
	if (status != 0)  {
		xil_printf("frmbufwr_setup 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("frmbufwr_init 1\n\r");
	status = frmbufwr_init(p_frmbufwr_1, XPAR_MIPI_CSI2_RX1_V_FRMBUF_WR_0_DEVICE_ID, p_intc, XPAR_INTC_0_V_FRMBUF_WR_1_VEC_ID);
	if (status != 0) {
		xil_printf("frmbufwr_init 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("frmbufwr_setup 1\n\r");
	status = frmbufwr_setup(p_frmbufwr_1, width, height, frame_rate, addr_1);
	if (status != 0)  {
		xil_printf("frmbufwr_setup 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int dual_procss_init(XVprocSs *p_csc_0, XVprocSs *p_scaler_0, XVprocSs *p_csc_1, XVprocSs *p_scaler_1, XGpioPs *p_emio, u32 width, u32 height, u32 frame_rate) {
	int status;

	xil_printf("dual_procss_reset\n\r");
	dual_procss_reset(p_emio);

	xil_printf("procss_init 0\n\r");
	status = procss_init(p_csc_0, XPAR_MIPI_CSI2_RX0_V_PROC_SS_CSC_DEVICE_ID, p_scaler_0, XPAR_MIPI_CSI2_RX0_V_PROC_SS_SCALER_DEVICE_ID);
	if (status != 0) {
		xil_printf("procss_init 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("procss_setup 0\n\r");
	status = procss_setup(p_csc_0, p_scaler_0, width, height, frame_rate);
	if (status != 0) {
		xil_printf("procss_setup 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	procss_csc(p_csc_0, 50, 50, 50, 50, 50, 50);

	xil_printf("procss_init 1\n\r");
	status = procss_init(p_csc_1, XPAR_MIPI_CSI2_RX1_V_PROC_SS_CSC_DEVICE_ID, p_scaler_1, XPAR_MIPI_CSI2_RX1_V_PROC_SS_SCALER_DEVICE_ID);
	if (status != 0) {
		xil_printf("procss_init 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("procss_setup 1\n\r");
	status = procss_setup(p_csc_1, p_scaler_1, width, height, frame_rate);
	if (status != 0) {
		xil_printf("procss_setup 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	procss_csc(p_csc_1, 50, 50, 50, 50, 50, 50);

	return XST_SUCCESS;
}

int dual_demosaic_init(XV_demosaic *p_demosaic_0, XV_demosaic *p_demosaic_1, XGpioPs *p_emio, u32 width, u32 height) {
	int status;

	xil_printf("dual_demosaic_reset\n\r");
	dual_demosaic_reset(p_emio);

	xil_printf("demosaic_init 0\n\r");
	status = demosaic_init(p_demosaic_0, XPAR_XV_DEMOSAIC_0_DEVICE_ID);
	if (status != 0) {
		xil_printf("demosaic_init 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	demosaic_setup(p_demosaic_0, width, height, 0);

	xil_printf("demosaic_init 1\n\r");
	status = demosaic_init(p_demosaic_1, XPAR_XV_DEMOSAIC_1_DEVICE_ID);
	if (status != 0) {
		xil_printf("demosaic_init 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	demosaic_setup(p_demosaic_1, width, height, 0);

	return XST_SUCCESS;
}

int dual_csi_init(XCsiSs *p_csi_0, XCsiSs *p_csi_1, XIntc *p_intc) {
	int status;

	xil_printf("csi_init 0\n\r");
	status = csi_init(p_csi_0, XPAR_CSI_0_DEVICE_ID, 4, XCSISS_ISR_ALLINTR_MASK, p_intc, XPAR_INTC_0_MIPICSISS_0_VEC_ID);
	if (status != 0) {
		xil_printf("csi_init 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	xil_printf("csi_init 1\n\r");
	status = csi_init(p_csi_1, XPAR_CSI_1_DEVICE_ID, 4, XCSISS_ISR_ALLINTR_MASK, p_intc, XPAR_INTC_0_MIPICSISS_1_VEC_ID);
	if (status != 0) {
		xil_printf("csi_init 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int gpio_5bit_init(XGpio *p_gpio) {
	int status;

	xil_printf("gpio 0 init\n\r");
	status = gpio_init(p_gpio, XPAR_AXI_GPIO_0_DEVICE_ID);
	if (status != 0) {
		xil_printf("gpio 0 init failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int dual_gammalut_init(XV_gamma_lut *p_gammalut_0, XV_gamma_lut *p_gammalut_1, XGpioPs *p_emio, u32 width, u32 height) {
	int status;

	xil_printf("dual_gammalut_reset\n\r");
	dual_gammalut_reset(p_emio);

	xil_printf("gammalut_init 0\n\r");
	status = gammalut_init(p_gammalut_0, XPAR_XV_GAMMA_LUT_0_DEVICE_ID);
	if (status != 0) {
		xil_printf("gammalut_init 0 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	gammalut_setup(p_gammalut_0, width, height, 0);

	xil_printf("gammalut_init 1\n\r");
	status = gammalut_init(p_gammalut_1, XPAR_XV_GAMMA_LUT_1_DEVICE_ID);
	if (status != 0) {
		xil_printf("gammalut_init 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	gammalut_setup(p_gammalut_1, width, height, 0);

	return XST_SUCCESS;
}

void sensor_init(struct stimx274 *p_imx274, u32 mode, u32 frame_rate, u32 exposure) {
	p_imx274->mode_index = mode;
	p_imx274->frame_interval.numerator = 1;
	p_imx274->frame_interval.denominator = frame_rate;

	xil_printf("imx264 set tp %X\n\r", imx274_set_test_pattern(p_imx274, 0));
	xil_printf("imx264 set vf %X\n\r", imx274_set_vflip(p_imx274, 0));
	xil_printf("imx264 set gain %X\n\r", imx274_set_gain(p_imx274, (22 * 1) << 8));//40 << 8));//20 << (8)));

	//((2048 << IMX274_GAIN_SHIFT) / (2048 - IMX274_GAIN_REG_MAX))

	xil_printf("imx264 mode %X\n\r", imx274_mode_regs(p_imx274, 0));
	xil_printf("imx264 set interval %X\n\r", imx274_set_frame_interval(p_imx274, p_imx274->frame_interval));
	xil_printf("imx264 set exp %X\n\r", imx274_set_exposure(p_imx274, exposure));//16636));//16636));
	xil_printf("imx264 start %X\n\r", imx274_start_stream(p_imx274));
}

int main() {
	XGpioPs emio;
	XScuGic apugic;
	XIntc intc;

	XV_FrmbufWr_l2 frmbufwr_0;
	XV_FrmbufWr_l2 frmbufwr_1;
	XVprocSs csc_0;
	XVprocSs scaler_0;
	XVprocSs csc_1;
	XVprocSs scaler_1;
	XV_gamma_lut gammalut_0;
	XV_gamma_lut gammalut_1;
	XV_demosaic demosaic_0;
	XV_demosaic demosaic_1;
	XCsiSs csi_0;
	XCsiSs csi_1;
	struct stimx274 imx274_0;
	struct stimx274 imx274_1;

	XGpio gpio;

	FATFS fatfs;

	u32 width = 3840;
	u32 height = 2160;
	u32 frame_rate = 60;
	u32 mode = 0;
	u32 bpp = 2;
	u32 capbit;
	u32 size = width * height * bpp;
	int frameid;
	char fname0[256];
	char fname1[256];

	int status;

	//

    init_platform();

    print("Hello World\n\r");

    //
    //

    xil_printf("Init SD\n\r");
	status = init_sd(&fatfs);
	if (status != XST_SUCCESS) {
		xil_printf("Init SD failed\n\r");
		return 0;
	}

    //

    status = common_init(&intc, &apugic, &emio);
    if (status != XST_SUCCESS) { return 0; }

    status = gpio_5bit_init(&gpio);
    if (status != XST_SUCCESS) { return 0; }

    //

    status = dual_iic_init(&imx274_0.iic_ex, &imx274_1.iic_ex, &intc);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_frmbufwr_init(&frmbufwr_0, &frmbufwr_1, &emio, &intc, width, height, frame_rate, (UINTPTR)DMA0_DST_ADDR_0, (UINTPTR)DMA1_DST_ADDR_0);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_procss_init(&csc_0, &scaler_0, &csc_1, &scaler_1, &emio, width, height, frame_rate);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_gammalut_init(&gammalut_0, &gammalut_1, &emio, width, height);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_demosaic_init(&demosaic_0, &demosaic_1, &emio, width, height);
    if (status != XST_SUCCESS) { return 0; }

    status = dual_csi_init(&csi_0, &csi_1, &intc);
    if (status != XST_SUCCESS) { return 0; }

	//

    xil_printf("intc_start\n\r");
	status = intc_start(&intc);
	if (status != 0) {
		xil_printf("intc_start failed! (%d)\n\r", status);
		return 0;
	}

	//

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

	//

	xil_printf("imx274 reset\n\r");
	dual_imx274_reset(&emio, 1);

	xil_printf("imx274 init\n\r");
	sensor_init(&imx274_0, mode, frame_rate, 16636 * 1);
	sensor_init(&imx274_1, mode, frame_rate, 16636 * 0.5);

	//

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

	//

	xil_printf("AOK!\n\r");

	frameid = 0;

	//"0:/img4k_0.raw"

	for (;;) {
		capbit <<= 5;
		capbit |= gpio_read_capture(&gpio);
		if ((capbit & 0x1) && !(capbit & 0x20)) {
			xil_printf("Write to SD\n\r");

			sprintf(fname0, "0:/i0_f%d.raw", frameid);
			sprintf(fname1, "0:/i1_f%d.raw", frameid);

			frameid++;

			g_lock = 1;
			xil_printf("Locked\n\r");

			Xil_DCacheInvalidate();
			Xil_DCacheFlush();

			xil_printf("Write 0 to SD %d (%s)\n\r", save_pic_sd(fname0, (UINTPTR)(g_target_0 ? DMA0_DST_ADDR_0 : DMA0_DST_ADDR_1), size), fname0);
			xil_printf("Write 1 to SD %d (%s)\n\r", save_pic_sd(fname1, (UINTPTR)(g_target_1 ? DMA1_DST_ADDR_0 : DMA1_DST_ADDR_1), size), fname1);

			g_lock = 0;

			sleep(1);
			capbit = 0;
		}
		else {
			sleep(1);
		}
	}

    cleanup_platform();
    return 0;
}
