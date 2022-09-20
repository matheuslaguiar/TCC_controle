/*
 * dwt.h
 *
 *  Created on: Sep 20, 2022
 *      Author: matheus.lemos
 */

#include <stdint.h>

#ifndef INC_DWT_H_
#define INC_DWT_H_

#define  ARM_CM_DEMCR      (*(uint32_t *)0xE000EDFC)
#define  ARM_CM_DWT_CTRL   (*(uint32_t *)0xE0001000)
#define  ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)

void DWT_Init(void);
uint32_t DWT_getMicroseconds(void);

#endif /* INC_DWT_H_ */
