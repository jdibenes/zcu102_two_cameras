/*
 * mipi.c
 *
 *  Created on: Feb 16, 2018
 *      Author: jcds
 */

#include "xcsiss.h"
#include "xil_printf.h"
#include "irq.h"
#include "xstatus.h"
#include "xparameters.h"

//#define CSI_LOG

static void CsiSs_DphyEventHandler(void *InstancePtr, u32 Mask) {
#ifdef CSI_LOG
	xil_printf("+===> DPHY Level Error detected.\n\r");
	if (Mask & XCSISS_ISR_SOTERR_MASK)     { xil_printf("Start of Transmission Error\n\r"); }
	if (Mask & XCSISS_ISR_SOTSYNCERR_MASK) { xil_printf("Start of Transmission Sync Error\n\r"); }
#endif
}

static void CsiSs_PktLvlEventHandler(void *InstancePtr, u32 Mask) {
#ifdef CSI_LOG
	xil_printf("+===> Packet Level Error detected.\n\r");
	if (Mask & XCSISS_ISR_ECC2BERR_MASK)  { xil_printf("2 bit ECC Error\n\r"); }
	if (Mask & XCSISS_ISR_ECC1BERR_MASK)  { xil_printf("1 bit ECC Error\n\r"); }
	if (Mask & XCSISS_ISR_CRCERR_MASK)    { xil_printf("Frame CRC Error\n\r"); }
	if (Mask & XCSISS_ISR_DATAIDERR_MASK) { xil_printf("Data Id Error\n\r"); }
#endif
}

static void CsiSs_ProtLvlEventHandler(void *InstancePtr, u32 Mask) {
#ifdef CSI_LOG
	xil_printf("+===> Packet Level Error detected.\n\r");
	if (Mask & XCSISS_ISR_VC3FSYNCERR_MASK) { xil_printf("VC3 Frame Sync Error\n\r"); }
	if (Mask & XCSISS_ISR_VC2FSYNCERR_MASK) { xil_printf("VC2 Frame Sync Error\n\r"); }
	if (Mask & XCSISS_ISR_VC1FSYNCERR_MASK) { xil_printf("VC1 Frame Sync Error\n\r"); }
	if (Mask & XCSISS_ISR_VC0FSYNCERR_MASK) { xil_printf("VC0 Frame Sync Error\n\r"); }
	if (Mask & XCSISS_ISR_VC3FLVLERR_MASK)  { xil_printf("VC3 Frame Level Error\n\r"); }
	if (Mask & XCSISS_ISR_VC2FLVLERR_MASK)  { xil_printf("VC2 Frame Level Error\n\r"); }
	if (Mask & XCSISS_ISR_VC1FLVLERR_MASK)  { xil_printf("VC1 Frame Level Error\n\r"); }
	if (Mask & XCSISS_ISR_VC0FLVLERR_MASK)  { xil_printf("VC0 Frame Level Error\n\r"); }
#endif
}

static void CsiSs_ErrEventHandler(void *InstancePtr, u32 Mask) {
#ifdef CSI_LOG
	xil_printf("+===> Other Errors detected.\n\r");
	if (Mask & XCSISS_ISR_WC_MASK)   { xil_printf("Word count corruption Error\n\r"); }
	if (Mask & XCSISS_ISR_ILC_MASK)  { xil_printf("Incorrect Lane Count Error\n\r"); }
	if (Mask & XCSISS_ISR_SLBF_MASK) { xil_printf("Stream line buffer full Error\n\r"); }
	if (Mask & XCSISS_ISR_STOP_MASK) { xil_printf("Stop Error \n\r"); }
#endif
}

static void CsiSs_SPktEventHandler(void *InstancePtr, u32 Mask) {
	XCsiSs *CsiSsInstance = (XCsiSs *)InstancePtr;
	if (Mask & XCSISS_ISR_SPFIFONE_MASK) { XCsiSs_GetShortPacket(CsiSsInstance); }

#ifdef CSI_LOG
	xil_printf("+===> Short Packet Event detected.\n\r");
	if (Mask & XCSISS_ISR_SPFIFONE_MASK) { xil_printf("Fifo not empty \n\r"); }
	if (Mask & XCSISS_ISR_SPFIFOF_MASK)  { xil_printf("Fifo Full\n\r"); }
#endif
}

static void CsiSs_FrameRcvdEventHandler(void *InstancePtr, u32 Mask) {
#ifdef CSI_LOG
	xil_printf("+=> Frame Received Event detected.\n\r");
#endif
}

int csi_init(XCsiSs *p_csi, u32 dev_id, u8 active_lanes, u32 intr_req, XIntc *p_intc, u8 vec_id) {
	XCsiSs_Config *cfg;
	int status;

	cfg = XCsiSs_LookupConfig(dev_id);
	if (cfg == NULL) { return -1; }

	status = XCsiSs_CfgInitialize(p_csi, cfg, cfg->BaseAddr);
	if (status != XST_SUCCESS) { return -2; }

	XCsiSs_Reset(p_csi);
	XCsiSs_Activate(p_csi, 0);
	XCsiSs_Configure(p_csi, active_lanes, intr_req);

	XCsiSs_SetCallBack(p_csi, XCSISS_HANDLER_DPHY,        CsiSs_DphyEventHandler,      p_csi);
	XCsiSs_SetCallBack(p_csi, XCSISS_HANDLER_PKTLVL,      CsiSs_PktLvlEventHandler,    p_csi);
	XCsiSs_SetCallBack(p_csi, XCSISS_HANDLER_PROTLVL,     CsiSs_ProtLvlEventHandler,   p_csi);
	XCsiSs_SetCallBack(p_csi, XCSISS_HANDLER_SHORTPACKET, CsiSs_SPktEventHandler,      p_csi);
	XCsiSs_SetCallBack(p_csi, XCSISS_HANDLER_FRAMERECVD,  CsiSs_FrameRcvdEventHandler, p_csi);
	XCsiSs_SetCallBack(p_csi, XCSISS_HANDLER_OTHERERROR,  CsiSs_ErrEventHandler,       p_csi);

	status = intc_connect(p_intc, vec_id, XCsiSs_IntrHandler, p_csi);
	if (status != XST_SUCCESS) { return -3; }

	return 0;
}

int csi_start(XCsiSs *p_csi) {
	return XCsiSs_Activate(p_csi, 1);
}
