#ifndef ADC_H
#define ADC_H

#include "stdint.h"

#define ADC_CHN_NUM					8//всего каналов ацп
#define ADC_CHN_POT_NUM			4//каналы ацп на регулируемых выходах пиропатронов

#define ADC_CALIBR_MIN_VAL	0x000
#define ADC_CALIBR_MIN_CUR	0.03 //Current - Amp.

#define ADC_CALIBR_MAX_VAL	0x700//0x7E0
#define ADC_CALIBR_MAX_CUR	1.0 //Current - Amp.

void ADC_Init(void);
float ADC_toCurrent(uint16_t adc_val);
#endif