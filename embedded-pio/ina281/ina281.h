#ifndef _INA281_H_
#define _INA281_H_

#include "adc.h"

/*
    A class which represents an instance of an INA281 Driver for measuring current across a
    shunt resistor. The analog output pin of the INA gives the voltage across the shunt resistor 
    multiplied by a scale factor (default 20). Current is calculated from voltage and resistance 
    using V = IR.
*/
class INA281Driver {
 private:
    uint32_t channel;
    float resistance;
    float scaleFactor;

 public:

   /*
        Creates a new INA281 Driver

        Constructor expects 2 arguments:
        channel - channel of the analog pin the INA is wired to
        resistance - resistance of the shunt resistor associated with the INA

        Optional argument:
        scaleFactor - the factor by which to divide the pin reading to get the correct voltage
    */
    INA281Driver(uint32_t channel, float resistance, float scaleFactor = 20);

    /*
        Retrieves the current current reading
    */
    float readCurrent();   

    /*
        Retrieves the current voltage reading
    */
    float readVoltage();
};

#endif