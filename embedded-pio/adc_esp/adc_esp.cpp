#include "adc_esp.h"
#include <string.h>

// Calibration characteristics
static esp_adc_cal_characteristics_t adc_chars;

adc_bits_width_t adc2_width = ADC_WIDTH_BIT_12;  // Since ADC2 can't be configured directly, we set the width here

esp_err_t esp32_adc_init(ESP32_ADC_Config* config) {
    esp_err_t ret = ESP_OK;
    
    // Configure ADC unit
    if (config->unit == ADC_UNIT_1) {
        ret = adc1_config_width(config->width);
        if (ret != ESP_OK) return ret;
    }
    else if (config->unit == ADC_UNIT_2) {
        adc2_width = config->width;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Characterize ADC for calibration
    esp_adc_cal_characterize(config->unit, config->attenuation, 
                             config->width, config->vref, &adc_chars);
    
    return ESP_OK;
}

esp_err_t esp32_adc_config_channel(adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation) {
    if (unit == ADC_UNIT_1) {
        return adc1_config_channel_atten((adc1_channel_t)channel, attenuation);
    } else if (unit == ADC_UNIT_2) {
        return adc2_config_channel_atten((adc2_channel_t)channel, attenuation);
    }
    return ESP_ERR_INVALID_ARG;
}

int esp32_adc_read_voltage(adc_unit_t unit, adc_channel_t channel, ESP32_ADC_Config* config) {
    int raw_value = esp32_adc_read_raw(unit, channel);
    if (raw_value < 0) return -1;
    
    // Convert raw value to voltage
    uint32_t voltage = esp_adc_cal_raw_to_voltage(raw_value, &adc_chars);
    return voltage;
}

int esp32_adc_read_raw(adc_unit_t unit, adc_channel_t channel) {
    int raw_value;
    
    if (unit == ADC_UNIT_1) {
        // Configure the channel attenuation if not already done
        // This should ideally be done during initialization for each channel
        adc1_channel_t adc1_channel = (adc1_channel_t)channel;
        raw_value = adc1_get_raw(adc1_channel);
    } else if (unit == ADC_UNIT_2) {
        // ADC2 can't be used when Wi-Fi is active
        esp_err_t ret = adc2_get_raw((adc2_channel_t)channel, adc2_width, &raw_value);
        if (ret != ESP_OK) {
            return -1;
        }
    } else {
        return -1;  // Invalid ADC unit
    }
    
    return raw_value;
}

float esp32_adc_normalize(int raw_value, ESP32_ADC_Config* config) {
    if (raw_value < 0) return -1.0f;
    
    int max_value;
    switch (config->width) {
        case ADC_WIDTH_BIT_9:  max_value = 511;  break;
        case ADC_WIDTH_BIT_10: max_value = 1023; break;
        case ADC_WIDTH_BIT_11: max_value = 2047; break;
        case ADC_WIDTH_BIT_12: max_value = 4095; break;
        default: return -1.0f;
    }
    
    return (float)raw_value / max_value;
}

esp_err_t esp32_adc_gpio_to_channel(int gpio_pin, adc_unit_t* unit, adc_channel_t* channel) {
    // Map GPIO pin to ADC channel
    // ADC1 channels
    if (gpio_pin == 36) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_0; return ESP_OK; }
    if (gpio_pin == 37) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_1; return ESP_OK; }
    if (gpio_pin == 38) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_2; return ESP_OK; }
    if (gpio_pin == 39) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_3; return ESP_OK; }
    if (gpio_pin == 32) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_4; return ESP_OK; }
    if (gpio_pin == 33) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_5; return ESP_OK; }
    if (gpio_pin == 34) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_6; return ESP_OK; }
    if (gpio_pin == 35) { *unit = ADC_UNIT_1; *channel = (adc_channel_t)ADC1_CHANNEL_7; return ESP_OK; }
    
    // ADC2 channels
    if (gpio_pin == 4)  { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_0; return ESP_OK; }
    if (gpio_pin == 0)  { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_1; return ESP_OK; }
    if (gpio_pin == 2)  { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_2; return ESP_OK; }
    if (gpio_pin == 15) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_3; return ESP_OK; }
    if (gpio_pin == 13) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_4; return ESP_OK; }
    if (gpio_pin == 12) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_5; return ESP_OK; }
    if (gpio_pin == 14) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_6; return ESP_OK; }
    if (gpio_pin == 27) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_7; return ESP_OK; }
    if (gpio_pin == 25) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_8; return ESP_OK; }
    if (gpio_pin == 26) { *unit = ADC_UNIT_2; *channel = (adc_channel_t)ADC2_CHANNEL_9; return ESP_OK; }
    
    return ESP_ERR_NOT_FOUND;
}
