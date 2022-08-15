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

// Setters
void control_setPulsoDir(int32_t value);
void control_setPulsoEsq(int32_t value);

#endif /* INC_CONTROL_H_ */
