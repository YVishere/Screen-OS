#include "ina281.h"

/*
    Initialize I2C bus member and parameters
*/
INA281Driver::INA281Driver(uint32_t channel, float resistance, float scaleFactor) {
    this->channel = channel;
    this->resistance = resistance;
    this->scaleFactor = scaleFactor; 
}

/*
    Retrieve new current reading
*/
float INA281Driver::readCurrent() {
    float measuredVoltage = readADC(this->channel) * 3.3;

    // voltage = measuredVoltage / scaleFactor
    // current = voltage / resistance
    float current = (measuredVoltage / scaleFactor) / resistance;

    return current;
}

/*
    Retrieve new voltage reading
*/
float INA281Driver::readVoltage() {
    float measuredVoltage = readADC(this->channel) * 3.3;
    float voltage = measuredVoltage / scaleFactor;
    return voltage;
}
