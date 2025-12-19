#include "dac.h"

DAC_HandleTypeDef dacHandle;
DAC_ChannelConfTypeDef dacConfig;


void initDAC(DAC_TypeDef* instance, uint32_t* channels, uint8_t num_channels) {
    dacHandle.Instance = instance;
    if (HAL_DAC_Init(&dacHandle) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }
    dacConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    dacConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    for (int i = 0; i < num_channels; i++) {
        if (HAL_DAC_ConfigChannel(&dacHandle, &dacConfig, channels[i]) != HAL_OK) {
            printf("channel %d not initialized\n", i);
        } else {
            printf("channel %d working\n", i);
        }
        HAL_DAC_Start(&dacHandle, channels[i]);
    }
}

void writeDAC(uint32_t channel, float value) {
    HAL_DAC_SetValue(&dacHandle, channel, DAC_ALIGN_12B_R, value * DAC_MAX_VALUE);
}