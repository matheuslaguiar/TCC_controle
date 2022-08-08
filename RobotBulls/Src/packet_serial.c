/*
 * packet_serial.c
 *
 *  Created on: Dec 8, 2021
 *      Author: mathe
 */

#include "bluetooth.h"
#include "packet_serial.h"
#include "usart.h"
#include <string.h>

#define ROBOCLAW_ADDRESS (0x80)

static uint8_t bytes_to_transmit[64];

/*
 * Calcula o crc16 (2 bytes de verificacao) para mandar os dados
 *
 * packet	-> ponteiro pro inicio do pacote
 * nBytes	-> tamanho do pacote desconsiderando o CRC
 *
 * */
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

/*
 * Envia efetivamente o comando para o roboclaw
 *
 * packet_in[0] = COMANDO
 * packet_in[1] -> packet_in[nBytes-1] = INFORMACOES ADICIONAIS (SE APLICAVEL)
 *
 * */
void packet_serial_enviaComando(uint8_t *packet_in, uint8_t *packet_out, uint8_t nBytes)
{
	// POSICAO VETOR:      0	  1->nBytes   nBytes+1  nBytes+2
	//					ADDRESS   packet_in    CRC16     CRC16

	memset(bytes_to_transmit, 0, sizeof(bytes_to_transmit)); // LIMPA VETOR

	bytes_to_transmit[0] = ROBOCLAW_ADDRESS;
	memcpy(&bytes_to_transmit[1], packet_in, nBytes);

	uint16_t crc = crc16(bytes_to_transmit, nBytes+1);
	bytes_to_transmit[nBytes+1] = (uint8_t) (crc >> 8);
	bytes_to_transmit[nBytes+2] = (uint8_t) crc;

	switch (packet_in[0]) {
	case PS_M1:
		HAL_UART_Transmit(&huart1, bytes_to_transmit, nBytes+3, 10); // nBytes+3 -> 1 endereco + 2 crc
		HAL_UART_Receive(&huart1, packet_out, 1, 10); // ESPERADO = [0xFF]
		break;

	case PS_M2:
		HAL_UART_Transmit(&huart1, bytes_to_transmit, nBytes+3, 10); // nBytes+3 -> 1 endereco + 2 crc
		HAL_UART_Receive(&huart1, packet_out, 1, 10); // ESPERADO = [0xFF]
		break;

	case PS_READ_BATTERY:
		HAL_UART_Transmit(&huart1, bytes_to_transmit, nBytes+1, 10); // nBytes+1 -> 1 endereco
		HAL_UART_Receive(&huart1, packet_out, 4, 10); // ESPERADO = [Value(2 bytes), CRC(2 bytes)]
		break;

	case PS_READ_CURRENT:
		HAL_UART_Transmit(&huart1, bytes_to_transmit, nBytes+1, 10); // nBytes+1 -> 1 endereco
		HAL_UART_Receive(&huart1, packet_out, 6, 10); // ESPERADO =  [M1 Current(2 bytes), M2 Currrent(2 bytes), CRC(2 bytes)]

		break;
	case PS_READ_FW_VERSION:
		HAL_UART_Transmit(&huart1, bytes_to_transmit, nBytes+1, 10); // nBytes+1 -> 1 endereco
		HAL_UART_Receive(&huart1, packet_out, 27, 10);

	default:
		bluetoothPrint((uint8_t *)"COMANDO INVALIDO\n");
	}
}
