/*
 * capture.h
 *
 *  Created on: Feb 2, 2021
 *      Author: jcds
 */

#ifndef SRC_CAPTURE_H_
#define SRC_CAPTURE_H_

#include "ff.h"
#include "imx274.h"
#include "s2mm_PL/frmbufwr.h"

int init_sd(FATFS *p_fatfs);
int save_file_sd(char *filename, UINTPTR src_addr, int size);
void capture_images(XV_FrmbufWr_l2 *p_frmbufwr_0, XV_FrmbufWr_l2 *p_frmbufwr_1, char *fname0, char *fname1, u32 size);
void sensor_init(struct stimx274 *p_imx274, u32 mode, u32 frame_rate, u32 exposure, u32 gain);

#endif /* SRC_CAPTURE_H_ */
