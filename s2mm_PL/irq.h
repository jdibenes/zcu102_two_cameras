/*
 * irq.h
 *
 *  Created on: Feb 19, 2018
 *      Author: jcds
 */

#ifndef SRC_IRQ_H_
#define SRC_IRQ_H_

#include "xintc.h"
#include "xscugic.h"

int intc_init(XIntc *p_intc, XScuGic *p_apugic);
int intc_connect(XIntc *p_intc, u8 id, XInterruptHandler handler, void *callbackref);
int intc_start(XIntc *p_intc);

#endif /* SRC_IRQ_H_ */
