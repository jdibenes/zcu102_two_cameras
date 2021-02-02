/*
 * sensor_i2c.h
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#ifndef SRC_SENSOR_I2C_H_
#define SRC_SENSOR_I2C_H_

#include "xintc.h"
#include "xiic.h"

#define IIC_CMD_FIFO_DEPTH 16
#define IMX274_IIC_ADDR 0x1A

typedef struct {
	XIic iic;
	int send_pending;
	int recv_pending;
} XIicEx;

typedef struct {
	s32 base;
	u8  data[IIC_CMD_FIFO_DEPTH];
	s32 count;
} iic_cmd_fifo;

int iic_write(XIicEx *p_iic_ex, u16 addr, u8 const *data, int byte_count);
int iic_read(XIicEx *p_iic_ex, u16 addr, u8 *data, int byte_count);
int iic_init(XIicEx *p_iic_ex, u16 dev_id, XIntc *p_intc, u8 vec_id, int sensor_addr);

void iic_cmd_reset(iic_cmd_fifo *cmd);
int iic_cmd_flush(XIicEx *p_iic_ex, iic_cmd_fifo *cmd);
int iic_cmd_wait(XIicEx *p_iic_ex, iic_cmd_fifo *cmd, u8 val);
int iic_cmd_push(XIicEx *p_iic_ex, iic_cmd_fifo *cmd, u16 addr, u8 val);

#endif /* SRC_SENSOR_I2C_H_ */
