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

// Setters
void control_setPulsoDir(int32_t value);
void control_setPulsoEsq(int32_t value);

/*
	Global variables
 */
uint32_t des_d, des_e;
uint32_t vel_d, vel_e; 	// velocidade escalar
uint32_t w_d, w_e;		// velocidade angular


#endif /* INC_CONTROL_H_ */
