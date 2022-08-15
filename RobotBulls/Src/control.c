/*
 * control.c
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */
#include "main.h"
#include "control.h"
#include "gpio.h"

int32_t pulso_dir = 0;
int32_t pulso_esq = 0;
uint32_t at;
uint32_t ultimo_t_dir = 0, delta_t_dir = 0;
uint32_t ultimo_t_esq = 0, delta_t_esq = 0;

uint32_t outros = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
	case ENC_DIR_Pin:
	{
		// TODO verificar os valores
		at = ARM_CM_DWT_CYCCNT;
		delta_t_dir = (at - ultimo_t_dir) /
				(HAL_RCC_GetHCLKFreq()/1000000);
		ultimo_t_dir = at;
		pulso_dir++;
		des_d += 4369;
		vel_d = (uint32_t)((uint64_t)4368777284/delta_t_dir);
		break;
	}
	case ENC_ESQ_Pin:
	{
		// TODO verificar os valores
		at = ARM_CM_DWT_CYCCNT;
		delta_t_esq = (at - ultimo_t_esq) /
				(HAL_RCC_GetHCLKFreq()/1000000);
		ultimo_t_esq = at;
		pulso_esq++;
		des_e += 4369;
		vel_e = (uint32_t)((uint64_t)4368777284/delta_t_esq);
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

