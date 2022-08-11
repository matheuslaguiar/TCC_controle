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

#endif /* INC_PERIPH_SPI_H_ */
