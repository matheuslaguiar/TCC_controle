/*
 * periph_spi.c
 *
 *  Created on: Aug 10, 2022
 *      Author: matheus.lemos
 */

#include "main.h"
#include "string.h"
#include "periph_spi.h"
#include "spi.h"

#define SPI_TIMEOUT		(0xFFFFFFFFU)

#ifndef SPI_BUFFER_SIZE
#define SPI_BUFFER_SIZE	(128)
#endif

static uint8_t spi_buffer[SPI_BUFFER_SIZE];

static void startComunication();
static void finishComunication();
static uint16_t crc16(uint8_t *packet, uint8_t nBytes);

uint32_t periph_spi_sendBuf(uint8_t * buffer, uint32_t length)
{
	HAL_StatusTypeDef status;
	uint16_t crc;

	// Busy
	if(hspi1.State != HAL_SPI_STATE_READY)
		return HAL_BUSY;

	// Length bigger than buffer size
	if(length > SPI_BUFFER_SIZE)
		return HAL_ERROR;

	// Clear SPI buffer
	memset(spi_buffer, 0x00, SPI_BUFFER_SIZE);

	// Copy buffer to SPI buffer
	memcpy(spi_buffer, buffer, length);

	// Calculate CRC and put at the end of buffer
	crc = crc16(spi_buffer, length);
	spi_buffer[length++] = (uint8_t) (crc >> 8);
	spi_buffer[length++] = (uint8_t) crc;

	// Data must be sent as a whole word (multiples of 4 bytes)
	length += 4 - (length%4);

	startComunication();
	status = HAL_SPI_Transmit(&hspi1, spi_buffer, length, SPI_TIMEOUT);
	finishComunication();

	return status;
}


static void startComunication(){
	// Reset chip select pin
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

	// Wait 1 ms to start sending data
	HAL_Delay(1);
}

static void finishComunication(){
	// Set chip select pin
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

static uint16_t crc16(uint8_t *packet, uint8_t nBytes) {
	uint16_t crc = 0;
	for (int byte = 0; byte < nBytes; byte++) {
		crc ^= ((uint16_t) packet[byte] << 8);

		for (uint8_t bit = 0; bit < 8; bit++) {
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc = crc << 1;
		}
	}
	return crc;
}
