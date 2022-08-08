/*
 * bluetooth.c
 *	Recursos para usar o bluetooth (módulo HC05)
 *  Created on: 1 de nov de 2021
 *      Author: Matheus Lemos de Aguiar
 */
#include "bluetooth.h"

/* indica se o próximo giro será à esquerda ou à direita */
uint8_t lado_dir;

/* buffer de saida*/
static char saida[20];
/*
 * Printa um vetor de caracteres via bluetooth
 *
 * */
void bluetoothPrint (uint8_t out[]) {
	HAL_UART_Transmit(&huart3, (uint8_t *) out, (uint16_t) strlen((char *) out), 10);
}

/*
 * Printa um valor (inteiro sem sinal de 32 bits) via bluetooth
 *
 * */
void bluetoothPrintVal (int32_t val) {
	uint8_t size = 0;
	uint8_t aux[2];
	aux[1] = '\0';
	if(val < 0) {
		val = -val;
		bluetoothPrint((uint8_t *) "-");
	}
	if(val == 0){
		bluetoothPrint((uint8_t *) "0");
		return;
	}
	while(val){
		saida[size++] = val%10+ '0';
		val/=10;
	}
	while(size){
		aux[0] = saida[--size];
		bluetoothPrint((uint8_t *) aux);
	}
}

/*
 * Seleciona jogada por bluetooth
 * Padrão: XAB
 * X -> 'D' (direita) ou 'E' (esquerda)
 * AB -> jogada
 * "01", "02", "03", ... ou "RC"
 * */
int32_t seleciona_bt () {
	int32_t jogada = -1;
	uint8_t aux[10];
	aux[0] = 255;
	do{
		HAL_UART_Receive(&huart3, (uint8_t *)aux, 3, 1000);

		if(aux[0] != 255) { // recebeu algo
			// definição esquerda ou direita
			bluetoothPrint((uint8_t *)"Lado: ");

			if(aux[0] == 'E') {
				lado_dir = 0;
				bluetoothPrint((uint8_t *) "E\n");
			}
			else {
				lado_dir = 1;
				bluetoothPrint((uint8_t *) "D\n");
			}

			// definição jogada

			bluetoothPrint((uint8_t *) "Jogada: ");
			// RC?
			if(aux[1] == 'R'){
				bluetoothPrint((uint8_t *) "RC\n");
				jogada = JOGADA_RC;

				return jogada;
			}

			// valor AB
			jogada = aux[1] - '0';
			jogada *= 10;
			jogada += aux[2] -'0';
			bluetoothPrintVal(jogada);
			bluetoothPrint((uint8_t *) "\n");
		}
	} while(jogada == -1);
	return jogada;
}

