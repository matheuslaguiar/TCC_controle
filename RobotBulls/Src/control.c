/*
 * control.c
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */
#include "main.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "motor.h"
#include "control.h"
#include "bluetooth.h"
#include "gpio.h"
#include "tim.h"
#include "dwt.h"

#define PI					(3.141592653589793)

#define RADIO_C_um			(22250)	// 44,5mm/2 = 22,25mm
#define DIST_B_um			(107000)	// 107 mm

#define TOLERANCIA_RAD 		(0.035)	// Aprox. 5,7°
#define	TOLERANCIA_um		(10000)	// 10mm

float kp=20, kd=5;
static int16_t potDir=0, potEsq=0;

int32_t pulso_dir = 0;
int32_t pulso_esq = 0;
int32_t ultimo_pulso_dir = 0;
int32_t ultimo_pulso_esq = 0;
//uint32_t ultimo_t_dir = 0;
//uint32_t ultimo_t_esq = 0;
int32_t des_d, des_e; // deslocamento da roda [um]
volatile int32_t vel_d, vel_e; // velocidade escalar [um/s]
volatile float w_d, w_e;		// velocidade angular [°/s]

typedef struct {
	int32_t d_x; // TODO analyze if it must be integer float
	int32_t d_y; // TODO analyze if it must be integer float
	float d_theta; // TODO analyze if it must be integer float
} derivadaPose_t;
derivadaPose_t d_pose;

typedef struct {
	int32_t x;
	int32_t y;
	float theta;
} Pose_t;

Pose_t pose = {0,0,0};
Pose_t poseSetPoint;

/* Private function prototypes -----------------------------------------------*/
static void get_error(Pose_t *p_err, Pose_t sp);
static void get_d_error(derivadaPose_t *p_dErr, Pose_t err, Pose_t last_err);

/* Public functions implementation -------------------------------------------*/

bool control_rotacao()
{
	static Pose_t err = {0,0,0}, last_err;
	static derivadaPose_t d_err = {0,0,0};

	last_err = err;

	// Get error
	get_error(&err, poseSetPoint);

	// Get error variation
	get_d_error(&d_err, err, last_err);

	if(err.theta>(-TOLERANCIA_RAD) && err.theta<(TOLERANCIA_RAD))
	{
		potDir = 0;
		potEsq = 0;
		return true;
	}

	potDir = (int16_t)(kp*err.theta + kd*d_err.d_theta);
	potEsq = -potDir;
	if(!potDir) { // muito proximo de 0, a ponto de arredondar pra 0
		if((kp*err.theta + kd*d_err.d_theta) > 0)
			potDir = 18, potEsq = -18;
	}

	// TODO adequar os valores de potencia
	if(potDir>63)
		potDir = 63;
	else if(potDir < -63)
		potDir = -63;
	if(potEsq>63)
		potEsq = 63;
	else if(potEsq < -63)
		potEsq = -63;

	if(potEsq>0 && potEsq<18)
		potEsq = 18;

	if(potEsq<0 && potEsq>-18)
		potEsq = -18;

	if(potDir>0 && potDir<18)
		potDir = 18;
	if(potDir<0 && potDir>-18)
		potDir = -18;

	// Envia potencia pros motores
	motorL(potEsq);
	motorR(potDir);

	return false;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// deslocamento = 1 pulso = 4368,77728 um
	switch (GPIO_Pin) {
	case ENC_DIR_Pin:
	{
		if(potDir < 0){
			pulso_dir--;
			des_d -= 4369;
		}
		else{
			pulso_dir++;
			des_d += 4369;
		}

//		vel_d = 436877728U / ((DWT_getMicroseconds()-ultimo_t_dir)/10);
//		ultimo_t_dir = DWT_getMicroseconds();
		break;
	}
	case ENC_ESQ_Pin:
	{
		if(potEsq < 0) {
			pulso_esq--;
			des_e -= 4369;
		}
		else{
			pulso_esq++;
			des_e += 4369;
		}
//		vel_e = 436877728U / ((DWT_getMicroseconds()-ultimo_t_esq)/10);
//		ultimo_t_esq = DWT_getMicroseconds();
		break;
	}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// TIM3 Period = 50 ms
	// T = (f_clock / (999+1))^(-1) * 3600 = (72MHz/1000)^(-1)*3600 = 50e-3
	if(htim == &htim3) {
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

		pose.theta += (d_pose.d_theta * 0.05);

		if(pose.theta > PI)
		{
			pose.theta = -6.28318530718 + pose.theta;
		}

		if(pose.theta < -PI)
		{
			pose.theta = 6.28318530718 + pose.theta;
		}

		// d_x = c*(wd+we)/2 * sen(theta)
		d_pose.d_x = (int32_t)((RADIO_C_um * ((w_d+w_e)/2)) * sin(pose.theta));
		pose.x += (int32_t)(d_pose.d_x*0.05);

		// d_y = c*(wd+we)/2 * cos(theta)
		d_pose.d_y = (int32_t)((RADIO_C_um * ((w_d+w_e)/2)) * cos(pose.theta));
		pose.y += (int32_t)(d_pose.d_y*0.05);

		ultimo_pulso_dir = pulso_dir;
		ultimo_pulso_esq = pulso_esq;

//		uint8_t buf[8];
		float g;

		// esse
//		bluetoothPrint((uint8_t *)"theta: ");
//		bluetoothPrintVal(((int32_t)(pose.theta*57.2958))%360);
//		bluetoothPrint((uint8_t *)"°\n");
//		bluetoothPrint((uint8_t *)"x: ");
//		bluetoothPrintVal(pose.x);
//		bluetoothPrint((uint8_t *)" um\n");

//		bluetoothPrintVal(pose.x/1000000);
//		snprintf((char *)buf,8,",%06u", (int)labs(pose.x%1000000));
//		bluetoothPrint(buf);
//		bluetoothPrint((uint8_t *)" m\n");

		// esse
//		bluetoothPrint((uint8_t *)"y: ");
//		bluetoothPrintVal(pose.y);
//		bluetoothPrint((uint8_t *)" um\n");

//		bluetoothPrintVal(pose.y/1000000);
//		snprintf((char *)buf,8,",%06u", (int)labs(pose.y%1000000));
//		bluetoothPrint(buf);
//		bluetoothPrint((uint8_t *)" m\n");

		// esse
//		bluetoothPrint((uint8_t *)"potDir: ");
//		bluetoothPrintVal(potDir);
//		bluetoothPrint((uint8_t *)"\npotEsq: ");
//		bluetoothPrintVal(potEsq);
//		bluetoothPrint((uint8_t *)"\n-----------------\n");

		if(poseSetPoint.theta != 0){
			g = (pose.theta*57.2958);
			bluetoothPrintVal((int32_t)g);
			bluetoothPrint((uint8_t *)",");
			bluetoothPrintVal((int32_t)(10*(g - (int32_t)g)));
			bluetoothPrint((uint8_t *)"\n");
		}
	}
}

/* Private function implementation -------------------------------------------*/
/**
 * @brief get e(t)
 *
 * @param[out] p_err	error pointer
 * @param[in]  sp		set point
 */
static void get_error(Pose_t *p_err, Pose_t sp)
{
	p_err->x = sp.x - pose.x;
	p_err->y = sp.y - pose.y;
	p_err->theta = sp.theta - pose.theta;
}

/**
 * @brief get e'(t)
 *
 * @param[out]	p_dErr		error variation pointer
 * @param[in]	last_err	last error value
 */
static void get_d_error(derivadaPose_t *p_dErr, Pose_t err, Pose_t last_err)
{
	p_dErr->d_x = err.x - last_err.x;
	p_dErr->d_y = err.y - last_err.y;
	p_dErr->d_theta = err.theta - last_err.theta;
}

/* Getters &  Setters *********************************************************/
int32_t control_getPulsoDir()
{
	return pulso_dir;
}

int32_t control_getPulsoEsq()
{
	return pulso_esq;
}

int32_t control_getVelD()
{
	return vel_d;
}

int32_t control_getVelE()
{
	return vel_e;
}

int32_t control_getDesD()
{
	return des_d;
}

int32_t control_getDesE()
{
	return des_e;
}

void control_setPose(int32_t x, int32_t y, float theta)
{
	pose.x = x;
	pose.y = y;
	pose.theta = theta;
}

void control_setPulsoDir(int32_t value)
{
	pulso_dir = value;
}

void control_setPulsoEsq(int32_t value)
{
	pulso_esq = value;
}

int32_t control_getPotDir(){
	return potDir;
}

int32_t control_getPotEsq(){
	return potEsq;
}

void control_setThetaSetPoint(float theta_sp){
	poseSetPoint.theta = theta_sp;
}
