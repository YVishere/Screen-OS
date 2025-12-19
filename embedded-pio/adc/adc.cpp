#include "adc.h"

ADC_HandleTypeDef adcHandle;
ADC_ChannelConfTypeDef adcConfig;

/* This function must be called in setup(). 
 * @param instance ADC#, where # is the handle number. 
 */
void initADC(ADC_TypeDef* instance) {
    adcHandle.Instance = instance;

    //deinitialize ADC
    if (HAL_ADC_DeInit(&adcHandle) != HAL_OK) {
        Error_Handler();
    }

    adcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;          /* Asynchronous clock mode, input ADC clock not divided */
    adcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
    adcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
    adcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    adcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
    adcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
    adcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
    adcHandle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
    adcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    adcHandle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
    adcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
    adcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
    adcHandle.Init.DMAContinuousRequests = DISABLE;                       /* DMA one-shot mode selected (not applied to this example) */
    adcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
    adcHandle.Init.OversamplingMode      = DISABLE;                       /* No oversampling */

    // initialize ADC
    if (HAL_ADC_Init(&adcHandle) != HAL_OK) {
        Error_Handler();
    }

    /* Run the ADC calibration in single-ended mode */
    if (HAL_ADCEx_Calibration_Start(&adcHandle, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    /*##-2- Configure ADC regular channel ######################################*/
    adcConfig.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCx_CHANNEL */
    adcConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES_5;    /* Sampling time (number of clock cycles unit) */
    adcConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
    adcConfig.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
    adcConfig.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */

}



/* Reads the analog value of the specified pin. 
 * @param channel ADC_CHANNEL_#, where # is the number of the channel
 * @return ADC voltage represented as a float in the range [0.0, 1.0]
 */
float readADC(uint32_t channel) {
     adcConfig.Channel = channel;                /* Sampled channel number */
    // configure channel
    if (HAL_ADC_ConfigChannel(&adcHandle, &adcConfig) != HAL_OK) {
        Error_Handler();
    }

     /*##-3- Start the conversion process #######################################*/
    if (HAL_ADC_Start(&adcHandle) != HAL_OK) {
        Error_Handler();
    }

    /*##-4- Wait for the end of conversion #####################################*/
    /*  For simplicity reasons, this example is just waiting till the end of the
        conversion, but application may perform other tasks while conversion
        operation is ongoing. */
    if (HAL_ADC_PollForConversion(&adcHandle, 10) != HAL_OK) {
        Error_Handler();
    }
    else {
        /* ADC conversion completed */
        /*##-5- Get the converted value of regular channel  ########################*/
        return (float)HAL_ADC_GetValue(&adcHandle) / ADC_MAX_VALUE;
    }
    
} 
