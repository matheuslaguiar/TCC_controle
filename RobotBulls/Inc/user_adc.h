/*
 * user_adc.h
 *
 *  Created on: 1 de nov de 2021
 *      Author: mathe
 */

#include "stdint.h"
#include "adc.h"

#ifndef INC_USER_ADC_H_
#define INC_USER_ADC_H_

/* TAMANHO DA BUFFER DO ADC */
#define ADC_BUF_LEN 16

/* QUANTIDADE DE CANAIS ADC */
#define ADC_NUM_CH 2

/* RANK DOS ADCs (definido no .ioc em ADC->Parameter Settings -> ADC_Regular_ConversionMode -> Rank */
#define RANK_ADC8 0
#define RANK_ADC9 1

uint32_t analogReadDMA (uint8_t rankCH);
void user_adc_Init();
#endif /* INC_USER_ADC_H_ */
