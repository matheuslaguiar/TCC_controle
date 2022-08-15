/*
 * pcs.c
 * 	Recursos para uso do Controle (PCS: Proportional Control System)
 *
 *  Created on: 1 de nov de 2021
 *      Author: Matheus Lemos de Aguiar
 */

#include "main.h"
#include "pcs.h"
#include "motor.h"

#ifndef TIME_OUT_PWM
#define TIME_OUT_PWM (30)
#endif

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
	uint32_t valor;
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
	valor = ARM_CM_DWT_CYCCNT;

	// espera terminar o pulso em HIGH
	while(HAL_GPIO_ReadPin(GPIO_PWM, GPIO_Pin_PWM)) {
		if(HAL_GetTick()-initial_tick > TIME_OUT_PWM) // FALHA
			return 1500;
	}

	// numero de ciclos em alta / f[MHz] = t_HIGH [us]
	valor = (ARM_CM_DWT_CYCCNT-valor) / (HAL_RCC_GetHCLKFreq()/1000000);

	// Regulagem do sinal
	if(valor < 1100)
		return 1000;
	if(valor > 1900)
		return 2000;
	if(valor > 1400 && valor < 1600)
		return 1500;

	return valor;
}
