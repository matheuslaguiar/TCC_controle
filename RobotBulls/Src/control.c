/*
 * control.c
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */

#include "control.h"
#include "gpio.h"

int32_t pulso_dir = 0;
int32_t pulso_esq = 0;
uint32_t outros = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
	case ENC_DIR_Pin:
	{
		pulso_dir++;
		break;
	}
	case ENC_ESQ_Pin:
	{
		pulso_esq++;
		break;
	}
	default:
		outros++;
	}
}

int32_t control_getPulsoDir()
{
	return pulso_dir;
}

int32_t control_getPulsoEsq()
{
	return pulso_esq;
}

