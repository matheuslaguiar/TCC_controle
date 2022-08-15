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

int32_t readPWM (GPIO_TypeDef *GPIO_PWM, uint16_t GPIO_Pin_PWM);

#endif /* INC_PCS_H_ */
