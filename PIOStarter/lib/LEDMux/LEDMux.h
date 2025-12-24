#ifdef HAS_LED_MUX
#pragma once

#include <Arduino.h>
#include "config.h"
#include "led_mux.pio.h"

// Clock divider to set general blink speed
static constexpr float MUX_PIO_CLK = 62500.0f;


class LEDMux{
    public:
        LEDMux();

        enum class Color {Red = 1, Yellow = 2, Green = 3, Blue = 4, Off = 0};
        
        void setup();

        bool estop();

        void updateLED(Color color);
        
        private:
        PIO pio;
        uint sm;
        uint32_t last_pio_delay = 0;
        
};

#endif