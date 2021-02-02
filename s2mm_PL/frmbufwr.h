/*
 * frmbufwr.h
 *
 *  Created on: Feb 21, 2018
 *      Author: jcds
 */

#ifndef SRC_FRMBUFWR_H_
#define SRC_FRMBUFWR_H_

#include "xv_frmbufwr_l2.h"
#include "xintc.h"
#include "xgpiops.h"

#define DMA0_DST_ADDR_0 (XPAR_DDR_MEM_BASEADDR + 0x20000000U)
#define DMA0_DST_ADDR_1 (XPAR_DDR_MEM_BASEADDR + 0x21000000U)
#define DMA1_DST_ADDR_0 (XPAR_DDR_MEM_BASEADDR + 0x22000000U)
#define DMA1_DST_ADDR_1 (XPAR_DDR_MEM_BASEADDR + 0x23000000U)

int frmbufwr_init(XV_FrmbufWr_l2 *p_frmbufwr, u16 dev_id, XIntc *p_intc, u8 vec_id);
int frmbufwr_setup(XV_FrmbufWr_l2 *p_frmbufwr, u32 width, u32 height, u32 frame_rate);
void frmbufwr_start(XV_FrmbufWr_l2 *p_frmbufwr);
int frmbufwr_stop(XV_FrmbufWr_l2 *p_frmbufwr);
void dual_frmbufwr_reset(XGpioPs *p_emio);
void frmbufwr_lock();
void frmbufwr_release();
u32 frmbufwr_getaddr(XV_FrmbufWr_l2 *p_frmbufwr);

#endif /* SRC_FRMBUFWR_H_ */
