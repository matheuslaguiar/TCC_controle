/*
 * pcs.c
 * 	Recursos para uso do Controle (PCS: Proportional Control System)
 *
 *  Created on: 1 de nov de 2021
 *      Author: Matheus Lemos de Aguiar
 */

#include "pcs.h"
#include "motor.h"

#ifndef TIME_OUT_PWM
#define TIME_OUT_PWM (30)
#endif

/*
 * Inicializa o DWT para contagem de ciclos
 * */
void DWT_Init(void) {
	if (ARM_CM_DWT_CTRL != 0) {        // See if DWT is available
		ARM_CM_DEMCR      |= 1 << 24;  // Set bit 24
		ARM_CM_DWT_CYCCNT  = 0;		   // Reset the count
		ARM_CM_DWT_CTRL   |= 1 << 0;   // Set bit 0
	}
}

/*
 * Le o valor PWM recebido do radio
 *
 * Parametros:	GPIO_PWM -> portal do pino desejado
 * 				GPIO_Pin_PWM -> Pino desejado
 *
 * Retorna a largura do pulso em microsegundos
 * MIN.            MAX.
 * 1000 < return < 2000
 * */
int32_t readPWM (GPIO_TypeDef *GPIO_PWM, uint16_t GPIO_Pin_PWM) {
	/*
	 * ESSA FUNCAO DEMORA NO MAX. 22 ms
	 * 2ms (PULSO ATUAL)
	 * 18ms (ESPERA NOVO PULSO)
	 * 2ms (PULSO MEDIDO)
	 *
	 * SE DEMORAR MAIS DO QUE ISSO, O RX ESTA DESCONECTADO
	 *
	 * TIME_OUT_PWM = 30 ms
	 * */
	uint32_t initial_tick;
	initial_tick = HAL_GetTick();

	// Espera encerrar o pulso atual (caso esteja no meio)
	while(HAL_GPIO_ReadPin(GPIO_PWM, GPIO_Pin_PWM)) {
		if(HAL_GetTick()-initial_tick > TIME_OUT_PWM) // FALHA
			return 1500;
	}

	// espera entrar em nivel HIGH novamente
	while(!HAL_GPIO_ReadPin(GPIO_PWM, GPIO_Pin_PWM)) {
		if(HAL_GetTick()-initial_tick > TIME_OUT_PWM) // FALHA
			return 1500;
	}

	// Reset the count
	ARM_CM_DWT_CYCCNT  = 0;

	// espera terminar o pulso em HIGH
	while(HAL_GPIO_ReadPin(GPIO_PWM, GPIO_Pin_PWM)) {
		if(HAL_GetTick()-initial_tick > TIME_OUT_PWM) // FALHA
			return 1500;
	}

	// numero de ciclos em alta / f[MHz] = t_HIGH [us]
	const int32_t valor = ARM_CM_DWT_CYCCNT / (HAL_RCC_GetHCLKFreq()/1000000);

	// Regulagem do sinal
	if(valor < 1100)
		return 1000;
	if(valor > 1900)
		return 2000;
	if(valor > 1400 && valor < 1600)
		return 1500;

	return valor;
}
