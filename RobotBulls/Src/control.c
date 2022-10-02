/*
 * control.c
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */
#include "main.h"
#include <math.h>
#include "control.h"
#include "bluetooth.h"
#include "gpio.h"
#include "tim.h"
#include "dwt.h"

#define RADIO_C_um			22250	// 44,5mm/2 = 22,25mm
#define DIST_B_um			107000	// 107 mm

int32_t pulso_dir = 0;
int32_t pulso_esq = 0;
uint32_t ultimo_pulso_dir = 0;
uint32_t ultimo_pulso_esq = 0;
//uint32_t ultimo_t_dir = 0;
//uint32_t ultimo_t_esq = 0;
uint32_t des_d, des_e; // deslocamento da roda [um]
volatile uint32_t vel_d, vel_e; // velocidade escalar [um/s]
volatile float w_d, w_e;		// velocidade angular [°/s]

struct {
	int32_t d_x; // TODO analyze if it must be integer float
	int32_t d_y; // TODO analyze if it must be integer float
	float d_theta; // TODO analyze if it must be integer float
} d_pose;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// deslocamento = 1 pulso = 4368,77728 um
	switch (GPIO_Pin) {
	case ENC_DIR_Pin:
	{
		pulso_dir++;
		des_d += 4369;
//		vel_d = 436877728U / ((DWT_getMicroseconds()-ultimo_t_dir)/10);
//		ultimo_t_dir = DWT_getMicroseconds();
		break;
	}
	case ENC_ESQ_Pin:
	{
		pulso_esq++;
		des_e += 4369;
//		vel_e = 436877728U / ((DWT_getMicroseconds()-ultimo_t_esq)/10);
//		ultimo_t_esq = DWT_getMicroseconds();
		break;
	}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static float theta = 0;
	static int a=0;

	// TIM3 Period = 50 ms
	// T = (f_clock / (999+1))^(-1) * 3600 = (72MHz/1000)^(-1)*3600 = 50e-3
	if(htim == &htim3) {
		a++;
		// vel = qtd_pulso*(circunferencia[um]/32) [um/50ms]
		// vel = qtd_pulso*(circunferencia[um]/32) / (50e-3) [um/s]
		// vel = qtd_pulso*(139801[um]/32) / (50e-3) [um/s]
		// vel = qtd_pulso*87376 [um/s]
		vel_d = (pulso_dir-ultimo_pulso_dir) * 87376;
		vel_e = (pulso_esq-ultimo_pulso_esq) * 87376;

		// w = qtd_pulso*(2pi/32) [rad/50ms]
		// w = qtd_pulso*(0,1963) / (50e-3) [rad/s]
		// w = qtd_pulso*3,92699 [rad/s]
		w_d = ((float) vel_d) / RADIO_C_um;
		w_e = ((float) vel_e) / RADIO_C_um;

		d_pose.d_theta = (((float)vel_d-(float)vel_e) / DIST_B_um);

		theta = theta + (d_pose.d_theta * 0.05);

		// d_x = c*(wd+we)/2 * sen(theta)
		d_pose.d_x = (int32_t)((RADIO_C_um * ((w_d+w_e)/2)) * sin(theta));
		// d_y = c*(wd+we)/2 * cos(theta)
		d_pose.d_y = (int32_t)((RADIO_C_um * ((w_d+w_e)/2)) * cos(theta));

		ultimo_pulso_dir = pulso_dir;
		ultimo_pulso_esq = pulso_esq;
	}
	/* Prints every 10*50ms = 0,5s */
	if(a == 10){
		a=0;
		bluetoothPrint("theta: ");
		bluetoothPrintVal(((int32_t)(theta*57.2958))%360);
		bluetoothPrint("°\n");
		bluetoothPrint("dx: ");
		bluetoothPrintVal(d_pose.d_x);
		bluetoothPrint(" um/s\n");
		bluetoothPrint("dy: ");
		bluetoothPrintVal(d_pose.d_y);
		bluetoothPrint(" um/s\n");
		bluetoothPrint("d theta: ");
		bluetoothPrintVal(((int32_t)(d_pose.d_theta*57.2958))%360);
		bluetoothPrint("°/s\n-------------\n");
	}
}

/* Getters &  Setters ********************************************************/
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

uint32_t control_getDesD()
{
	return des_d;
}

uint32_t control_getDesE()
{
	return des_e;
}

void control_setPulsoDir(int32_t value)
{
	pulso_dir = value;
}

void control_setPulsoEsq(int32_t value)
{
	pulso_esq = value;
}
