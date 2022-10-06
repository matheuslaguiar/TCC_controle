/*
 * control.h
 *
 *  Created on: Aug 5, 2022
 *      Author: aluno
 */

#include <stdint.h>

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_


// Getters
int32_t control_getPulsoDir();
int32_t control_getPulsoEsq();
uint32_t control_getVelD();
uint32_t control_getVelE();
uint32_t control_getDesD();
uint32_t control_getDesE();

// Setters
void control_setPose(int32_t x, int32_t y, float theta);
void control_setPulsoDir(int32_t value);
void control_setPulsoEsq(int32_t value);

#endif /* INC_CONTROL_H_ */
