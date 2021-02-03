/*
 * frmbufwr.c
 *
 *  Created on: Feb 16, 2018
 *      Author: jcds
 */

#include "xv_frmbufwr_l2.h"
#include "xintc.h"
#include "xgpiops.h"
#include "irq.h"
#include "emio.h"
#include "sleep.h"
#include "frmbufwr.h"

static u32 g_lock     = 0;
static u32 g_target_0 = 0;
static u32 g_target_1 = 0;

static void frmbufwr_irq_handler(void *data) {
	XV_FrmbufWr_l2 *inst;
	u32 addr;

#ifdef FRMBUFWR_LOG
	xil_printf("frmbufwr IRQ done\n\r");
#endif

	if (g_lock) { return; }

	inst = (XV_FrmbufWr_l2 *)data;

	switch (inst->FrmbufWr.Config.DeviceId) {
	case 0: g_target_0 ^= 1; addr = g_target_0 ? DMA0_DST_ADDR_1 : DMA0_DST_ADDR_0; break;
	case 1: g_target_1 ^= 1; addr = g_target_1 ? DMA1_DST_ADDR_1 : DMA1_DST_ADDR_0; break;
	}

	XVFrmbufWr_SetBufferAddr(inst, addr);
}

int frmbufwr_init(XV_FrmbufWr_l2 *p_frmbufwr, u16 dev_id, XIntc *p_intc, u8 vec_id) {
	int status;

	status = XVFrmbufWr_Initialize(p_frmbufwr, dev_id);
	if (status != XST_SUCCESS) { return -1; }

	status = intc_connect(p_intc, vec_id, (XInterruptHandler)XVFrmbufWr_InterruptHandler, p_frmbufwr);
	if (status != XST_SUCCESS) { return -2; }

	XVFrmbufWr_SetCallback(p_frmbufwr, frmbufwr_irq_handler, p_frmbufwr);

	return 0;
}

int frmbufwr_setup(XV_FrmbufWr_l2 *p_frmbufwr, u32 width, u32 height, u32 frame_rate) {
	XVidC_VideoStream stream_in;
	XVidC_VideoMode video_mode;
	XVidC_VideoTiming const *p_timing;
	XVidC_ColorFormat mem_fmt;
	u32 bpp;
	u32 stride;
	int status;

	video_mode = XVidC_GetVideoModeId(width, height, frame_rate, 0);
	p_timing = XVidC_GetTimingInfo(video_mode);

	stream_in.PixPerClk     = p_frmbufwr->FrmbufWr.Config.PixPerClk;
	stream_in.ColorDepth    = p_frmbufwr->FrmbufWr.Config.MaxDataWidth;
	stream_in.ColorFormatId = XVIDC_CSF_YCRCB_422;
	stream_in.VmId          = video_mode;
	stream_in.Timing        = *p_timing;
	stream_in.FrameRate     = frame_rate;

	mem_fmt = XVIDC_CSF_MEM_YUYV8;
	bpp = 2;

	stride = width * bpp;

	status = XVFrmbufWr_SetMemFormat(p_frmbufwr, stride, mem_fmt, &stream_in);
	if (status != XST_SUCCESS) { return -3; }

	status = XVFrmbufWr_SetBufferAddr(p_frmbufwr, frmbufwr_getaddr(p_frmbufwr));
	if (status != XST_SUCCESS) { return -4; }

	return 0;
}

void frmbufwr_start(XV_FrmbufWr_l2 *p_frmbufwr) {
	XVFrmbufWr_InterruptEnable(p_frmbufwr);
	XVFrmbufWr_Start(p_frmbufwr);
}

int frmbufwr_stop(XV_FrmbufWr_l2 *p_frmbufwr) {
	XVFrmbufWr_InterruptDisable(p_frmbufwr);
	return XVFrmbufWr_Stop(p_frmbufwr);
}

void dual_frmbufwr_reset(XGpioPs *p_emio) {
	emio_set_frmbufwr_reset(p_emio, 0);
	usleep(EMIO_RESET_DELAY_US);
	emio_set_frmbufwr_reset(p_emio, 1);
	usleep(EMIO_RESET_DELAY_US);
}

void frmbufwr_lock() {
	g_lock = 1;
}

void frmbufwr_release() {
	g_lock = 0;
}

u32 frmbufwr_getaddr(XV_FrmbufWr_l2 *p_frmbufwr) {
	u32 addr = 0;

	switch (p_frmbufwr->FrmbufWr.Config.DeviceId) {
	case 0: addr = g_target_0 ? DMA0_DST_ADDR_0 : DMA0_DST_ADDR_1; break;
	case 1: addr = g_target_1 ? DMA1_DST_ADDR_0 : DMA1_DST_ADDR_1; break;
	}

	return addr;
}
