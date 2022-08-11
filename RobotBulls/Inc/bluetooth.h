/*
 * bluetooth.h
 *	Recursos para usar o bluetooth (módulo HC05)
 *  Created on: 1 de nov de 2021
 *      Author: Matheus Lemos de Aguiar
 */

#include "string.h"
#include "usart.h"

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

/* JOGADAS SUMO 3KG*/
typedef enum {
	CONFIG = -1,
	JOGADA_RC,
	TESTE_SPI,
} jogada3Kg_TypeDef;
/* Variáveis */
extern uint8_t lado_dir;

/* FUNÇÕES */
void bluetoothPrint (uint8_t out[]);
void bluetoothPrintVal (int32_t val);
int32_t seleciona_bt (void);

#endif /* INC_BLUETOOTH_H_ */
