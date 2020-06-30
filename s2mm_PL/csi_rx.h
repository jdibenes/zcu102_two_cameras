/*
 * csi_rx.h
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#ifndef SRC_CSI_RX_H_
#define SRC_CSI_RX_H_

#include "xcsiss.h"

int csi_init(XCsiSs *p_csi, u32 dev_id, u8 active_lanes, u32 intr_req, XIntc *p_intc, u8 vec_id);
int csi_start(XCsiSs *p_csi);

#endif /* SRC_CSI_RX_H_ */
