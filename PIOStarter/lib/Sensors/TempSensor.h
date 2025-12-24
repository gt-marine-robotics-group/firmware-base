#ifdef HAS_SENSORS

#pragma once


#include <Arduino.h>
#include "config.h"
#include <Wire.h>
#include "SparkFun_Si7021_Breakout_Library.h"

class TempSensor{
    public:
        TempSensor();

        SI7021 sensor;

        void setup();

        bool estop();

        uint16_t readData();

    private:
        
};

#endif