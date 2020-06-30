/*
 * procss.c
 *
 *  Created on: Feb 20, 2018
 *      Author: jcds
 */

#include "xvprocss.h"
#include "xgpiops.h"
#include "sleep.h"
#include "emio.h"

int procss_init(XVprocSs *p_csc, u16 dev_id_csc, XVprocSs *p_scaler, u16 dev_id_scaler) {
	XVprocSs_Config *cfg_csc;
	XVprocSs_Config *cfg_scaler;
	int status;

	//

	memset(p_scaler, 0, sizeof(XVprocSs));

	cfg_scaler = XVprocSs_LookupConfig(dev_id_scaler);
	if (cfg_scaler == NULL) { return -3; }

	XVprocSs_LogReset(p_scaler);

	status = XVprocSs_CfgInitialize(p_scaler, cfg_scaler, cfg_scaler->BaseAddress);
	if (status != XST_SUCCESS) { return -4; }

	//

	memset(p_csc, 0, sizeof(XVprocSs));

	cfg_csc = XVprocSs_LookupConfig(dev_id_csc);
	if (cfg_csc == NULL) { return -1; }

	XVprocSs_LogReset(p_csc);

	status = XVprocSs_CfgInitialize(p_csc, cfg_csc, cfg_csc->BaseAddress);
	if (status != XST_SUCCESS) { return -2; }

	//

	return 0;
}

int procss_setup(XVprocSs *p_csc, XVprocSs *p_scaler, u32 width, u32 height, u32 frame_rate) {
	XVidC_VideoMode video_mode;
	XVidC_VideoTiming const *p_timing;
	XVidC_VideoStream csc_stream_in;
	XVidC_VideoStream csc_stream_out;
	XVidC_VideoStream scaler_stream_in;
	XVidC_VideoStream scaler_stream_out;

	video_mode = XVidC_GetVideoModeId(width, height, frame_rate, 0);
	p_timing = XVidC_GetTimingInfo(video_mode);
	if (p_timing == NULL) { return -5; }

	//

	memset(&csc_stream_in, 0, sizeof(csc_stream_in));

	csc_stream_in.VmId          = video_mode;
	csc_stream_in.Timing        = *p_timing;
	csc_stream_in.ColorFormatId = XVIDC_CSF_RGB;
	csc_stream_in.ColorDepth    = p_csc->Config.ColorDepth;
	csc_stream_in.PixPerClk     = p_csc->Config.PixPerClock;
	csc_stream_in.FrameRate     = frame_rate;

	XVprocSs_SetVidStreamIn(p_csc, &csc_stream_in);

	memset(&csc_stream_out, 0, sizeof(csc_stream_out));

	csc_stream_out.VmId          = video_mode;
	csc_stream_out.Timing        = *p_timing;
	csc_stream_out.ColorFormatId = XVIDC_CSF_YCRCB_444;
	csc_stream_out.ColorDepth    = p_csc->Config.ColorDepth;
	csc_stream_out.PixPerClk     = p_csc->Config.PixPerClock;
	csc_stream_out.FrameRate     = frame_rate;

	XVprocSs_SetVidStreamOut(p_csc, &csc_stream_out);

	//

	memset(&scaler_stream_in, 0, sizeof(scaler_stream_in));

	scaler_stream_in.VmId          = video_mode;
	scaler_stream_in.Timing        = *p_timing;
	scaler_stream_in.ColorFormatId = XVIDC_CSF_YCRCB_444;
	scaler_stream_in.ColorDepth    = p_scaler->Config.ColorDepth;
	scaler_stream_in.PixPerClk     = p_scaler->Config.PixPerClock;
	scaler_stream_in.FrameRate     = frame_rate;

	XVprocSs_SetVidStreamIn(p_scaler, &scaler_stream_in);

	memset(&scaler_stream_out, 0, sizeof(scaler_stream_out));

	scaler_stream_out.VmId          = video_mode;
	scaler_stream_out.Timing        = *p_timing;
	scaler_stream_out.ColorFormatId = XVIDC_CSF_YCRCB_422;
	scaler_stream_out.ColorDepth    = p_scaler->Config.ColorDepth;
	scaler_stream_out.PixPerClk     = p_scaler->Config.PixPerClock;
	scaler_stream_out.FrameRate     = frame_rate;

	XVprocSs_SetVidStreamOut(p_scaler, &scaler_stream_out);

	//

	return 0;
}

void procss_csc(XVprocSs *p_csc, s32 brightness, s32 contrast, s32 saturation, s32 rgain, s32 ggain, s32 bgain) {
	XVprocSs_SetPictureBrightness(p_csc, brightness);
	XVprocSs_SetPictureContrast  (p_csc, contrast);
	XVprocSs_SetPictureSaturation(p_csc, saturation);

	XVprocSs_SetPictureGain(p_csc, XVPROCSS_COLOR_CH_Y_RED,    rgain);
	XVprocSs_SetPictureGain(p_csc, XVPROCSS_COLOR_CH_CB_GREEN, ggain);
	XVprocSs_SetPictureGain(p_csc, XVPROCSS_COLOR_CH_CR_BLUE,  bgain);

	XVprocSs_SetPictureColorStdIn (p_csc, XVIDC_BT_2020);
	XVprocSs_SetPictureColorStdOut(p_csc, XVIDC_BT_2020);
	XVprocSs_SetPictureColorRange (p_csc, XVIDC_CR_0_255);
}

int procss_start(XVprocSs *p_csc, XVprocSs *p_scaler) {
	int status;

	status = XVprocSs_SetSubsystemConfig(p_scaler);
	if (status != XST_SUCCESS) { return -6; }

	status = XVprocSs_SetSubsystemConfig(p_csc);
	if (status != XST_SUCCESS) { return -7; }

	return 0;
}

void dual_procss_reset(XGpioPs *p_emio) {
	emio_set_ssscaler_reset(p_emio, 0);
	usleep(EMIO_RESET_DELAY_US);
	emio_set_ssscaler_reset(p_emio, 1);
	usleep(EMIO_RESET_DELAY_US);

	emio_set_sscsc_reset(p_emio, 0);
	usleep(EMIO_RESET_DELAY_US);
	emio_set_sscsc_reset(p_emio, 1);
	usleep(EMIO_RESET_DELAY_US);
}
