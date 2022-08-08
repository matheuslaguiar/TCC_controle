/*
 * pcs.h
 * 	Recursos para uso do Controle (PCS: Proportional Control System)
 *
 *  Created on: 1 de nov de 2021
 *      Author: Matheus Lemos de Aguiar
 */
#include "stdint.h"
#include "gpio.h"

#ifndef INC_PCS_H_
#define INC_PCS_H_

// Para uso do DWT
#define  ARM_CM_DEMCR      (*(uint32_t *)0xE000EDFC)
#define  ARM_CM_DWT_CTRL   (*(uint32_t *)0xE0001000)
#define  ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)


void DWT_Init(void);
int32_t readPWM (GPIO_TypeDef *GPIO_PWM, uint16_t GPIO_Pin_PWM);

#endif /* INC_PCS_H_ */
