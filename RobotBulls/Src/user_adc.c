/*
 * user_adc.c
 *
 *  Created on: 1 de nov de 2021
 *      Author: mathe
 */

#include "user_adc.h"

/* buffer ADC */
volatile static uint32_t adc_buf[ADC_BUF_LEN];

void user_adc_Init(void){
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adc_buf, ADC_BUF_LEN);
}

/*
 * Leitura canal ADC
 * rankCH -> rank do canal (definido no .ioc em ADC_Regular_ConversionMode)
 * */
uint32_t analogReadDMA (uint8_t rankCH) {
	uint32_t i, ret = 0;
	for(i = rankCH;i <ADC_BUF_LEN;i += ADC_NUM_CH){
		ret += adc_buf[i];
	}

	return ret * ADC_NUM_CH / ADC_BUF_LEN;
}
