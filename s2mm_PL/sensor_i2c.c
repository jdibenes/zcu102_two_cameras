/*
 * sensor_i2c.c
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#include "xiic.h"
#include "xintc.h"
#include "irq.h"
#include "sensor_i2c.h"
#include "sleep.h"
#include "xil_printf.h"

//#define IIC_DEBUG

// ----------------------------------------------------------------------------

void iic_send_handler(XIic *p_iic) {
	(((XIicEx *)p_iic)->send_pending) = FALSE;
}

void iic_recv_handler(XIic *p_iic) {
	(((XIicEx *)p_iic)->recv_pending) = FALSE;
}

void iic_status_handler(XIic *p_iic, int event) {
	xil_printf("IIC IRQ status: %X\n\r", event);
}

// ----------------------------------------------------------------------------

#ifdef IIC_DEBUG
static int iic_dbg_send(XIic *p_iic, u8 *data, int byte_count) {
	int n;
	for (n = 0; n < byte_count; ++n) { xil_printf("%.2X\n\r", data[n]); }
	iic_send_handler(p_iic);
	return XST_SUCCESS;
}

static int iic_dbg_recv(XIic *p_iic, u8 *data, int byte_count) {
	int n;
	for (n = 0; n < byte_count; ++n) { data[n] = 0; }
	xil_printf("Read %d\n\r", byte_count);
	iic_recv_handler(p_iic);
	return XST_SUCCESS;
}

static int iic_dbg_start(XIic *p_iic) {
	xil_printf("IIC Start\n\r");
	return XST_SUCCESS;
}

static int iic_dbg_stop(XIic *p_iic) {
	xil_printf("IIC Stop\n\r");
	return XST_SUCCESS;
}

static int iic_dbg_busy(XIic *p_iic) {
	return FALSE;
}

static void iic_dbg_set_options(XIic *p_iic, u32 options) {
	xil_printf("Options %X\n\r", options);
}

#define XIic_MasterSend iic_dbg_send
#define XIic_MasterRecv iic_dbg_recv
#define XIic_IsIicBusy  iic_dbg_busy
#define XIic_Start      iic_dbg_start
#define XIic_Stop       iic_dbg_stop
#define XIic_SetOptions iic_dbg_set_options
#endif

// ----------------------------------------------------------------------------

static int iic_send(XIicEx *p_iic_ex, u8 *data, int byte_count) {
	XIic *p_iic = &p_iic_ex->iic;
	int status;

	p_iic_ex->send_pending = TRUE;
	status = XIic_MasterSend(p_iic, data, byte_count);
	if (status != XST_SUCCESS) { return XST_FAILURE; }
	while (p_iic_ex->send_pending == TRUE);

	return XST_SUCCESS;
}

static int iic_recv(XIicEx *p_iic_ex, u8 *data, int byte_count) {
	XIic *p_iic = &p_iic_ex->iic;
	int status;

	p_iic_ex->recv_pending = TRUE;
	status = XIic_MasterRecv(p_iic, data, byte_count);
	if (status != XST_SUCCESS) { return XST_FAILURE; }
	while (p_iic_ex->recv_pending == TRUE);

	return XST_SUCCESS;
}

// ----------------------------------------------------------------------------

static void iic_addr_split(u16 addr, u8 *wb) {
	wb[0] = (u8)(addr >> 8);
	wb[1] = (u8) addr;
}

int iic_write(XIicEx *p_iic_ex, u16 addr, u8 const *data, int byte_count) {
	XIic *p_iic = &p_iic_ex->iic;
	int size = byte_count + 2;
	u8 wb[size];
	int status;

	iic_addr_split(addr, wb);
	memcpy(&wb[2], data, byte_count);

	status = XIic_Start(p_iic);
	if (status != XST_SUCCESS) { return -1; }

	status = iic_send(p_iic_ex, wb, size);
	if (status != XST_SUCCESS) { return -2; }
	while (XIic_IsIicBusy(p_iic) == TRUE);

	status = XIic_Stop(p_iic);
	if (status != XST_SUCCESS) { return -3; }

	return 0;
}

int iic_read(XIicEx *p_iic_ex, u16 addr, u8 *data, int byte_count) {
	XIic *p_iic = &(p_iic_ex->iic);
	int size = 2;
	u8 wb[size];
	int status;

	iic_addr_split(addr, wb);

	status = XIic_Start(p_iic);
	if (status != XST_SUCCESS) { return -4; }

	XIic_SetOptions(p_iic, XII_REPEATED_START_OPTION);
	status = iic_send(p_iic_ex, wb, size);
	if (status != XST_SUCCESS) { return -5; }

	XIic_SetOptions(p_iic, 0);
	status = iic_recv(p_iic_ex, data, byte_count);
	if (status != XST_SUCCESS) { return -6; }

	while (XIic_IsIicBusy(p_iic) == TRUE);

	status = XIic_Stop(p_iic);
	if (status != XST_SUCCESS) { return -7; }

	return 0;
}

// ----------------------------------------------------------------------------

void iic_cmd_reset(iic_cmd_fifo *cmd) {
	(cmd->base) = -1;
	(cmd->count) = 0;
}

int iic_cmd_flush(XIicEx *p_iic_ex, iic_cmd_fifo *cmd) {
	int ret;
	if ((cmd->count) <= 0) { return 0; }
	ret = iic_write(p_iic_ex, cmd->base, cmd->data, cmd->count);
	iic_cmd_reset(cmd);
	return ret;
}

int iic_cmd_wait(XIicEx *p_iic_ex, iic_cmd_fifo *cmd, u8 val) {
	int ret;
	ret = iic_cmd_flush(p_iic_ex, cmd);
	usleep((unsigned long)val * 1000UL);
	return ret;
}

int iic_cmd_push(XIicEx *p_iic_ex, iic_cmd_fifo *cmd, u16 addr, u8 val) {
	int ret;

	if ((cmd->base) < 0)                           { goto _F_access; }
	else if (((cmd->base) + (cmd->count)) != addr) { goto _N_access; }
	else                                           { goto _S_access; }

_N_access:
	ret = iic_cmd_flush(p_iic_ex, cmd);
	if (ret != 0) { return ret; }

_F_access:
	(cmd->base) = addr;

_S_access:
	(cmd->data)[(cmd->count)++] = val;
	if ((cmd->count) < IIC_CMD_FIFO_DEPTH) { return 0; }
	return iic_cmd_flush(p_iic_ex, cmd);
}

// ----------------------------------------------------------------------------

int iic_init(XIicEx *p_iic_ex, u16 dev_id, XIntc *p_intc, u8 vec_id, int sensor_addr) {
	XIic *p_iic = &p_iic_ex->iic;
	XIic_Config *cfg;
	int status;

	cfg = XIic_LookupConfig(dev_id);
	if (cfg == NULL) { return -1; }

	status = XIic_CfgInitialize(p_iic, cfg, cfg->BaseAddress);
	if (status != XST_SUCCESS) { return -2; }

	status = intc_connect(p_intc, vec_id, XIic_InterruptHandler, p_iic);
	if (status != XST_SUCCESS) { return -3;}

	XIic_SetSendHandler(  p_iic, p_iic, (XIic_Handler)      iic_send_handler  );
	XIic_SetRecvHandler(  p_iic, p_iic, (XIic_Handler)      iic_recv_handler  );
	XIic_SetStatusHandler(p_iic, p_iic, (XIic_StatusHandler)iic_status_handler);

	status = XIic_SetAddress(p_iic, XII_ADDR_TO_SEND_TYPE, sensor_addr);
	if (status != XST_SUCCESS) { return -4; }

	return 0;
}
