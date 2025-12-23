#pragma once
#include <Arduino.h>


namespace config {
    #ifdef HAS_MOTORS
        // Pinout (Pin Definitions)
        static constexpr uint8_t AI1 = 15;
        static constexpr uint8_t AI2 = 14;
        static constexpr uint8_t POT_IN = 26;
        static constexpr uint8_t PWM_OUT = 18;
    #endif

    #ifdef HAS_ESTOP
        static constexpr uint8_t ESTOP_PIN = 13;
        static constexpr uint8_t ESTOP_LED = 12;
    #endif

    #ifdef HAS_INDICATOR_LED
        static constexpr uint8_t LED_PIN = 16;     // The pin that blinks
        static constexpr uint8_t TRIGGER_PIN = 17; // The input pin we are watching
    #endif

    #ifdef HAS_SENSORS
        static constexpr uint8_t TEMP_SDA = 10;
        static constexpr uint8_t TEMP_SCL = 11;
    #endif
}