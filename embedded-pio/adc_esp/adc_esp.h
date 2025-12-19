#ifndef ESP32_ADC_H
#define ESP32_ADC_H

#include <driver/adc.h>
#include <esp_adc_cal.h>

// ADC configuration struct
typedef struct {
    adc_unit_t unit;           // ADC unit (ADC1 or ADC2)
    adc_bits_width_t width;    // ADC bit width (9-12 bits)
    adc_atten_t attenuation;   // ADC attenuation
    uint32_t vref;             // Reference voltage in mV (typically 1100mV)
} ESP32_ADC_Config;

/**
 * Initialize ADC with the given configuration
 * @param config ADC configuration
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp32_adc_init(ESP32_ADC_Config* config);

/**
 * Configure a specific ADC channel with the given attenuation
 * @param unit ADC unit (ADC1 or ADC2)
 * @param channel ADC channel
 * @param attenuation Attenuation level
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp32_adc_config_channel(adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation);

/**
 * Read ADC value from the specified channel
 * @param unit ADC unit (ADC1 or ADC2)
 * @param channel ADC channel
 * @param config ADC configuration used for calibration
 * @return Voltage in millivolts, or -1 on error
 */
int esp32_adc_read_voltage(adc_unit_t unit, adc_channel_t channel, ESP32_ADC_Config* config);

/**
 * Read raw ADC value from the specified channel
 * @param unit ADC unit (ADC1 or ADC2)
 * @param channel ADC channel
 * @return Raw ADC value (0-4095 for 12-bit), or -1 on error
 */
int esp32_adc_read_raw(adc_unit_t unit, adc_channel_t channel);

/**
 * Convert raw ADC value to normalized float (0.0-1.0)
 * @param raw_value Raw ADC value
 * @param config ADC configuration for bit width
 * @return Normalized value between 0.0 and 1.0
 */
float esp32_adc_normalize(int raw_value, ESP32_ADC_Config* config);

/**
 * Get ADC channel for a GPIO pin
 * @param gpio_pin GPIO pin number
 * @param unit Pointer to store the ADC unit
 * @param channel Pointer to store the ADC channel
 * @return ESP_OK if mapping successful, error code otherwise
 */
esp_err_t esp32_adc_gpio_to_channel(int gpio_pin, adc_unit_t* unit, adc_channel_t* channel);

#endif // ESP32_ADC_H