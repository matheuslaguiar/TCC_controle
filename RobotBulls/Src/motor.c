/*
 * motor.c
 *
 *  Created on: Nov 6, 2021
 *      Author: mathe
 */
#include "motor.h"
#include "packet_serial.h"

static uint8_t bytes_to_transmit[64], bytes_to_receive[64];

/*
 * Controla motor direito
 *  -63 <= v <= 63
 *  -63 ~ -1 -> backward
 *  0 -> stop
 *  1 ~ 63 -> forward
 *
 * */
void motorR (int16_t v)
{
	bytes_to_transmit[0] = PS_M2; // COMANDO MOTOR 2 = DIREITO
	bytes_to_transmit[1] = (uint8_t)(v + 64);

	packet_serial_enviaComando(bytes_to_transmit, bytes_to_receive, 2);
}

/*
 * Controla motor esquerdo
 *  -63 <= v <= 63
 *  -63 ~ -1 -> backward
 *  0 -> stop
 *  1 ~ 63 -> forward
 *
 * */
void motorL (int16_t v)
{
	bytes_to_transmit[0] = PS_M1; // COMANDO MOTOR 1 = ESQUERDO
	bytes_to_transmit[1] = (uint8_t)(v + 64);

	packet_serial_enviaComando(bytes_to_transmit, bytes_to_receive, 2);
}

/*
 * Lê corrente dos motores
 * passo: 10mA
 *
 * exemplo: 251 = 2,51A
 *
 * */
void readMotorCurrent(uint16_t *Current_ML, uint16_t *Current_MR)
{
	bytes_to_transmit[0] = PS_READ_CURRENT;
	packet_serial_enviaComando(bytes_to_transmit, bytes_to_receive,1);
	*Current_ML = ((uint16_t)bytes_to_receive[0])<<8;
	*Current_ML += (uint16_t)bytes_to_receive[1];

	*Current_MR = ((uint16_t)bytes_to_receive[2])<<8;
	*Current_MR += (uint16_t)bytes_to_receive[3];
}

/*
 * Lê a tensão da bateria do motor
 * passo: 0,1 V
 *
 * exemplo: 251 = 25,1V
 * */
uint16_t readBattery() {
	bytes_to_transmit[0] = PS_READ_BATTERY;
	packet_serial_enviaComando(bytes_to_transmit, bytes_to_receive,1);
	return (((uint16_t)bytes_to_receive[0])<<8) + (uint16_t)bytes_to_receive[1];
}
