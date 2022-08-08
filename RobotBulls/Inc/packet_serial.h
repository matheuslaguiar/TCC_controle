/*
 * packet_serial.h
 *
 *  Created on: Dec 8, 2021
 *      Author: mathe
 */
#include <stdint.h>
#define PS_M1				6
#define PS_M2				7
#define PS_READ_FW_VERSION	21
#define PS_READ_BATTERY		24
#define PS_READ_CURRENT		49


#ifndef INC_PACKET_SERIAL_H_
#define INC_PACKET_SERIAL_H_

void packet_serial_enviaComando(uint8_t *packet_in, uint8_t *packet_out, uint8_t nBytes);

#endif /* INC_PACKET_SERIAL_H_ */
