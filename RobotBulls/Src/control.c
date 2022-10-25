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

#define TOLERANCIA_MAX_RAD	(0.0872665)	// 0.0872665 = 5°
#define TOLERANCIA_RAD 		(0.035)		// 0.035 = 2°
#define	TOLERANCIA_um		(50000)	// 50mm

float kp_rotacao=20, kd_rotacao=5;
float kp_reta=0.00006, kd_reta=0.00006;
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
	int32_t d_x;
	int32_t d_y;
	float d_theta;
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
static float get_thetaSp(float delta_x, float delta_y);

static int32_t get_dist(Pose_t A, Pose_t B);
/* Public functions implementation -------------------------------------------*/
bool control_process()
{
	static float theta_sp, err_theta;


	theta_sp = get_thetaSp((float)(poseSetPoint.x-pose.x),
			(float)(poseSetPoint.y-pose.y));

	err_theta = theta_sp-pose.theta;

	bluetoothPrint((uint8_t *)"theta_sp: ");
	bluetoothPrintVal(theta_sp*57.2958);
	bluetoothPrint((uint8_t *)" °\n");
	// theta com erro > TOLERANCIA_MAX_RAD
	if(!((err_theta>0 && err_theta<TOLERANCIA_MAX_RAD)
			|| (err_theta<0 && err_theta>-TOLERANCIA_MAX_RAD)))
	{
		motorL(potDir=0);
		motorR(potEsq=0);
		HAL_Delay(50);
		control_setThetaSetPoint(theta_sp);
		while(!control_rotacao())
		{
			HAL_Delay(10);
		}
	}

	return control_reta();
}

bool control_reta()
{
	static int32_t err = 0, last_err;
	static int32_t d_err = 0;

	static uint32_t a = 0;
	last_err = err;
	err = get_dist(poseSetPoint, pose);
	d_err = err - last_err;

	if((err < TOLERANCIA_um && err >= 0) || (err > -TOLERANCIA_um && err < 0))
	{
		motorL(potDir = 0);
		motorR(potEsq = 0);
		HAL_Delay(50);
		return true;
	}
	if(a++ == 5){
		bluetoothPrint((uint8_t *)"Err: ");
		bluetoothPrintVal(err);
		bluetoothPrint((uint8_t *)" um\n");
		bluetoothPrint((uint8_t *)"dErr: ");
		bluetoothPrintVal(d_err);
		bluetoothPrint((uint8_t *)" um/s\n");
		bluetoothPrint((uint8_t *)"Pot Dir: ");
		bluetoothPrintVal(potDir);
		bluetoothPrint((uint8_t *)"\n");
		bluetoothPrint((uint8_t *)"Pot Esq: ");
		bluetoothPrintVal(potEsq);
		bluetoothPrint((uint8_t *)"\n");
		a = 0;
	}

	potDir = (int16_t)(kp_reta*err + kd_reta*d_err);
	potEsq = potDir;

	if(!potDir) { // muito proximo de 0, a ponto de arredondar pra 0
		potDir = 15, potEsq = 15;
	}

	// adequar os valores de potencia
	if(potDir>63)
		potDir = 63;
	else if(potDir < -63)
		potDir = -63;
	if(potEsq>63)
		potEsq = 63;
	else if(potEsq < -63)
		potEsq = -63;

	if(potEsq>0 && potEsq<15)
		potEsq = 15;

	if(potEsq<0 && potEsq>-15)
		potEsq = -15;

	if(potDir>0 && potDir<15)
		potDir = 15;
	if(potDir<0 && potDir>-15)
		potDir = -15;

	// Envia potencia pros motores
	motorL(potEsq);
	motorR(potDir);

	return false;
}

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
		motorR(potDir = 0);
		motorL(potEsq = 0);
		HAL_Delay(50);
		return true;
	}

	potDir = (int16_t)(kp_rotacao*err.theta + kd_rotacao*d_err.d_theta);
	potEsq = -potDir;
	if(!potDir) { // muito proximo de 0, a ponto de arredondar pra 0
		if((kp_rotacao*err.theta + kd_rotacao*d_err.d_theta) > 0)
			potDir = 18, potEsq = -18;
		else
			potDir = -18, potEsq = 18;
	}

	// adequar os valores de potencia
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
	static int32_t ultPotDir, ultPotEsq;
	// deslocamento = 1 pulso = 4368,77728 um
	switch (GPIO_Pin) {
	case ENC_DIR_Pin:
	{
		if(potDir < 0){
			pulso_dir--;
			des_d -= 4369;
			ultPotDir = potDir;
		}
		else if (potDir){
			pulso_dir++;
			des_d += 4369;
			ultPotDir = potDir;
		}
		// SITUAÇÕES EM INÉRCIA :
		else if (ultPotDir < 0){
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
			ultPotEsq = potEsq;
		}
		else if (potEsq){
			pulso_esq++;
			des_e += 4369;
			ultPotEsq = potEsq;
		}
		// SITUAÇÕES EM INÉRCIA :
		else if (ultPotEsq < 0){
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
//		float g;

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

		if(poseSetPoint.y != 0){
			bluetoothPrint((uint8_t *)"posX: ");
			bluetoothPrintVal(pose.x);
			bluetoothPrint((uint8_t *)" um\n");

			bluetoothPrint((uint8_t *)"posY: ");
			bluetoothPrintVal(pose.y);
			bluetoothPrint((uint8_t *)" um\n\n");

//			g = (pose.theta*57.2958);
//			bluetoothPrintVal((int32_t)g);
//			bluetoothPrint((uint8_t *)",");
//			bluetoothPrintVal((int32_t)(10*(g - (int32_t)g)));
//			bluetoothPrint((uint8_t *)"\n");
		}
	}
}

/* Private functions implementation ------------------------------------------*/

static float get_thetaSp(float delta_x, float delta_y)
{
	// delta_y == 0
	if(delta_y < 1e-5 && delta_y > -1e-5){
		if(delta_x > 0)
			return PI/2;
		else
			return -PI/2;
	}

	// delta_y < 0
	if(delta_y < 0){
		float theta_ret;
		theta_ret = atan2f(delta_x,delta_y) + PI;

		if(theta_ret > PI)
			theta_ret = -6.28318530718 + theta_ret;
		else if(theta_ret < -PI)
			theta_ret = 6.28318530718 + theta_ret;

		return theta_ret;
	}

	// delta_y > 0
	return atan2f(delta_x,delta_y);
}

static int32_t get_dist(Pose_t A, Pose_t B)
{
	return (int32_t)(sqrt((((float)(A.x-B.x))*(A.x-B.x)) + (((float)(A.y-B.y))*(A.y-B.y))));
}

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


/* Getters &  Setters --------------------------------------------------------*/
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

void control_setXYSetPoint(int32_t x_sp, int32_t y_sp){
	poseSetPoint.x = x_sp;
	poseSetPoint.y = y_sp;
}
