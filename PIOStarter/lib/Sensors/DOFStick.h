#ifdef HAS_SENSORS

#pragma once


#include <Arduino.h>
#include "config.h"
#include <Wire.h>
// #include <SparkFunLSM9DS1.h>

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!




class DOFStick{
    public:
        DOFStick();

        // LSM9DS1 imu;
        Adafruit_LSM9DS1 imu;

        void setup();

        bool estop();

        uint16_t readData();

    private:
        
};

#endif