#ifdef HAS_INDICATOR_LED

#pragma once

#include <Arduino.h>
#include "config.h"
#include "blink_input.pio.h"

// Clock divider to set general blink speed
static constexpr float PIO_CLK = 62500.0f;

class LEDPIO{
    public:
        LEDPIO();

        void setup();

        bool estop();

        void updateBlink(uint8_t pio_delay);

    private:
        uint sm;
        uint32_t last_pio_delay = 0;
};

#endif