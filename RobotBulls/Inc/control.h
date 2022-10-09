/*
 * control.h
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

bool control_process();
void control_setThetaSetPoint(float theta_sp);
void control_setXYSetPoint(int32_t x_sp, int32_t y_sp);
bool control_reta();
bool control_rotacao();

// Getters
int32_t control_getPulsoDir();
int32_t control_getPulsoEsq();
int32_t control_getVelD();
int32_t control_getVelE();
int32_t control_getDesD();
int32_t control_getDesE();
int32_t control_getPotDir();
int32_t control_getPotEsq();

// Setters
void control_setPose(int32_t x, int32_t y, float theta);
void control_setPulsoDir(int32_t value);
void control_setPulsoEsq(int32_t value);

#endif /* INC_CONTROL_H_ */
