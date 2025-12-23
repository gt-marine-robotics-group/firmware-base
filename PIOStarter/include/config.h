#pragma once
#include <Arduino.h>


namespace config {
    #ifdef BOARD_ALPHA
        #define HAS_MOTORS 1
        #define HAS_INDICATOR_LED 1
        #define HAS_ESTOP 1

        // Pinout (Pin Definitions)
        static constexpr uint8_t AI1 = 15;
        static constexpr uint8_t AI2 = 14;
        static constexpr uint8_t POT_IN = 26;
        static constexpr uint8_t PWM_OUT = 18;

        static constexpr uint8_t ESTOP_PIN = 13;
        static constexpr uint8_t ESTOP_LED = 12;
        
        static constexpr uint8_t LED_PIN = 16;     // The pin that blinks
        static constexpr uint8_t TRIGGER_PIN = 17; // The input pin we are watching

    #endif

    #ifdef BOARD_BETA
        #define USE_TEMPERATURE_SENSOR
        #define USE_PRESSURE_SENSOR

        static constexpr uint8_t PIN_I2C_SDA 4
        static constexpr uint8_t PIN_I2C_SCL 5
    #endif
}