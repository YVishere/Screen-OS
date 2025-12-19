#ifndef __DAC_H__
#define __DAC_H__
#include <Arduino.h>

#define DAC_MAX_VALUE 4096 // because we use 12 bit DAC

void initDAC(DAC_TypeDef* instance, uint32_t* channels, uint8_t num_channels);
void writeDAC(uint32_t channel, float value);

#endif