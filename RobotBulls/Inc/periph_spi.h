/*
 * periph_spi.h
 *
 *  Created on: Aug 10, 2022
 *      Author: matheus.lemos
 */

#include <stdint.h>

#ifndef INC_PERIPH_SPI_H_
#define INC_PERIPH_SPI_H_

uint32_t periph_spi_sendBuf(uint8_t * buffer, uint32_t length);
void periph_spi_sendBatteryVoltage();
void periph_spi_sendMotorCurrent();
void periph_spi_sendLeftMotorPower(int8_t value);
void periph_spi_sendRightMotorPower(int8_t value);
void periph_spi_sendMotorSpeed();
void periph_spi_sendSelectedMove(uint8_t move);

#endif /* INC_PERIPH_SPI_H_ */
