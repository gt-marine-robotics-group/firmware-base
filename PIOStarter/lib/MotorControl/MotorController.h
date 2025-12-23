#pragma once
#ifdef HAS_MOTORS

#include <Arduino.h>
#include "config.h"

constexpr int ADC_MAX    = 1023;
constexpr int ADC_CENTER = 512;
constexpr int PWM_MAX    = 255;
constexpr int DEADBAND   = 50;   // adjust to remove jitter

class MotorController{
    public:
        MotorController();

        void setup();

        bool estop();

        // Returns Motor Delay
        uint16_t spinMotor(uint8_t pin, int PWM);

        void spinMotors(uint8_t* pins, int* PWMs, uint16_t* results, uint8_t count);
        
    private:
};

#endif