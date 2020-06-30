/*
 * irq.c
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#include "xintc.h"
#include "xscugic.h"
#include "xil_exception.h"

#define PL_PS_IRQ0_0_ID   121
#define PL_PS_IRQ0_0_PRIO 0xA0
#define PL_PS_IRQ0_0_TRIG 0x01

int intc_init(XIntc *p_intc, XScuGic *p_apugic) {
	XScuGic_Config *cfg;
	int status;

	status = XIntc_Initialize(p_intc, XPAR_AXI_INTC_0_DEVICE_ID);
	if (status != XST_SUCCESS) { return -1; }

	cfg = XScuGic_LookupConfig(XPAR_PSU_ACPU_GIC_DEVICE_ID);
	if (cfg == NULL) { return -2; }

	status = XScuGic_CfgInitialize(p_apugic, cfg, cfg->CpuBaseAddress);
	if (status != XST_SUCCESS) { return -3; }

	XScuGic_SetPriorityTriggerType(p_apugic, PL_PS_IRQ0_0_ID, PL_PS_IRQ0_0_PRIO, PL_PS_IRQ0_0_TRIG);

	status = XScuGic_Connect(p_apugic, PL_PS_IRQ0_0_ID, (Xil_ExceptionHandler)XIntc_InterruptHandler, p_intc);
	if (status != XST_SUCCESS) { return -4; }

	XScuGic_Enable(p_apugic, PL_PS_IRQ0_0_ID);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, p_apugic);
	Xil_ExceptionEnable();

	return 0;
}

int intc_connect(XIntc *p_intc, u8 id, XInterruptHandler handler, void *callbackref) {
	int status;

	status = XIntc_Connect(p_intc, id, handler, callbackref);
	if (status != XST_SUCCESS) { return XST_FAILURE; }
	XIntc_Enable(p_intc, id);
	return XST_SUCCESS;
}

int intc_start(XIntc *p_intc) {
	int status;

	status = XIntc_Start(p_intc, XIN_REAL_MODE);
	if (status != XST_SUCCESS) { return XST_FAILURE; }
	return XST_SUCCESS;
}
