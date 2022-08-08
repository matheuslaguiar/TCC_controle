/*
 * control.c
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */

#include "control.h"
#include "gpio.h"

int32_t pulso_dir_A = 0;
int32_t pulso_esq_A = 0;
uint32_t outros = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
	case ENC_DIR_A_Pin:
	{
		if(HAL_GPIO_ReadPin(ENC_DIR_B_GPIO_Port, ENC_DIR_B_Pin))
			pulso_dir_A++;
		else
			pulso_dir_A--;
		break;
	}
	case ENC_ESQ_A_Pin:
	{
		if(HAL_GPIO_ReadPin(ENC_ESQ_B_GPIO_Port, ENC_ESQ_B_Pin))
			pulso_esq_A++;
		else
			pulso_esq_A--;
		break;
	}
	default:
		outros++;
	}
}

int32_t control_getPulsoDirA()
{
	return pulso_dir_A;
}

int32_t control_getPulsoEsqA()
{
	return pulso_esq_A;
}

