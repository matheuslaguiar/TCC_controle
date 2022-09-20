/*
 * dwt.c
 *
 *  Created on: Sep 20, 2022
 *      Author: matheus.lemos
 */

#include "dwt.h"

/*
 * Inicializa o DWT para contagem de ciclos
 * */
void DWT_Init(void) {
	if (ARM_CM_DWT_CTRL != 0) {        // See if DWT is available
		ARM_CM_DEMCR      |= 1 << 24;  // Set bit 24
		ARM_CM_DWT_CYCCNT  = 0;		   // Reset the count
		ARM_CM_DWT_CTRL   |= 1 << 0;   // Set bit 0
	}
}

/*
 * Lê a quantidade de ciclos em microssegundos
 *
 * IMPORTANTE: PARA UM CLOCK DE 72 MHz
 *
 * */
uint32_t DWT_getMicroseconds(void)
{
	return ARM_CM_DWT_CYCCNT / 72U;
}
