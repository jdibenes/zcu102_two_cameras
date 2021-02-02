/*
 * init.c
 *
 *  Created on: Feb 2, 2021
 *      Author: jcds
 */

#include "xil_printf.h"
#include "init.h"

int common_init(XIntc *p_intc, XScuGic *p_apugic, XGpioPs *p_emio, XGpio *p_gpio) {
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

	xil_printf("gpio 0 init\n\r");
	status = gpio_init(p_gpio, XPAR_AXI_GPIO_0_DEVICE_ID);
	if (status != 0) {
		xil_printf("gpio 0 init failed! (%d)\n\r", status);
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

int dual_frmbufwr_init(XV_FrmbufWr_l2 *p_frmbufwr_0, XV_FrmbufWr_l2 *p_frmbufwr_1, XGpioPs *p_emio, XIntc *p_intc, u32 width, u32 height, u32 frame_rate) {
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
	status = frmbufwr_setup(p_frmbufwr_0, width, height, frame_rate);
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
	status = frmbufwr_setup(p_frmbufwr_1, width, height, frame_rate);
	if (status != 0)  {
		xil_printf("frmbufwr_setup 1 failed! (%d)\n\r", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
