#ifdef HAS_SENSORS

#pragma once


#include <Arduino.h>
#include "config.h"
#include <Wire.h>
#include "Adafruit_Si7021.h"

class I2CSensor{
    public:
        I2CSensor();

        Adafruit_Si7021 sensor = Adafruit_Si7021(&Wire1);

        void setup();

        bool estop();

        uint16_t readData();

    private:
        
};

#endif