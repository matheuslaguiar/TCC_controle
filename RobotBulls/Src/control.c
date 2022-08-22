/*
 * control.c
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */
#include "main.h"
#include "control.h"
#include "gpio.h"
#include "tim.h"

int32_t pulso_dir = 0;
int32_t pulso_esq = 0;
uint32_t ultimo_pulso_dir = 0;
uint32_t ultimo_pulso_esq = 0;
uint32_t des_d, des_e; // deslocamento da roda [um]
uint32_t vel_d, vel_e; 	// velocidade escalar [um/s]

uint32_t outros = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
	case ENC_DIR_Pin:
	{
		pulso_dir++;
		des_d += 4369;
		break;
	}
	case ENC_ESQ_Pin:
	{
		pulso_esq++;
		des_e += 4369;
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

uint32_t control_getVelD()
{
	return vel_d;
}

uint32_t control_getVelE()
{
	return vel_e;
}

void control_setPulsoDir(int32_t value)
{
	pulso_dir = value;
}

void control_setPulsoEsq(int32_t value)
{
	pulso_esq = value;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// TIM3 Period = 100 ms
	// T = (f_clock / (999+1))^(-1) * 7200 = (72MHz/1000)^(-1)*7200 = 100e-3
	if(htim == &htim3) {
		// vel = qtd_pulso*(circunferencia[um]/32) [um/100ms]
		// vel = qtd_pulso*(circunferencia[um]/32) * 1e1 [um/s]
		// vel = qtd_pulso*(139801[um]/32)*1e1 [um/s]
		// vel = qtd_pulso*43688 [um/s]
		vel_d = (pulso_dir-ultimo_pulso_dir) * 43688;
		vel_e = (pulso_esq-ultimo_pulso_esq) * 43688;
		ultimo_pulso_dir = pulso_dir;
		ultimo_pulso_esq = pulso_esq;
	}
}
