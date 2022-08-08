/*
 * motor.h
 *
 *  Created on: Nov 6, 2021
 *      Author: mathe
 */
#include <stdint.h>

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

void motorR (int16_t v);
void motorL (int16_t v);

void readMotorCurrent(uint16_t *Current_ML, uint16_t *Current_MR);

uint16_t readBattery();

#endif /* INC_MOTOR_H_ */
