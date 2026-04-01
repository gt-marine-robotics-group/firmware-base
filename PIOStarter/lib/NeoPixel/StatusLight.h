#pragma once

/**
 * @file LEDPIO.h
 * @author Jason Hsiao
 * @date 12/24/2025
 * @version 1.0
 *
 * @brief Header for using NeoPixel Status Light.
 *
 * @note This code is the lab code from ECE4180, I know that it works, 
 * but don't know if this implementation is fine.
 * @see config.h for the hardware definitions like GPIO mapping and 
 * tuned clock dividers.
 */

#ifdef HAS_STATUS_LIGHT

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixel(config::NUM_LEDS, config::LED_PIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief 
 * Acts as a higher-level C++ wrapper for using the Adafruit NeoPixel library.
 * This class manages initializing and updating the NEOPIXEL. 
 * @note Only compiled if the HAS_INDICATOR_LED flag is defined in the build environment.
 */
class StatusLight{
    public:
        StatusLight(){};

        void setup() {
            pixel.begin();           // Initialize NeoPixel
            pixel.setBrightness(20); // Make it less bright!
        }

        void on() {
            pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Display Red
            pixel.show();
        }

        void off(){
            pixel.setPixelColor(0, pixel.Color(0, 0, 0));   // Off
            pixel.show();
        }

        void blink() {
            pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Display Red
            pixel.show();
            delay(500);

            pixel.setPixelColor(0, pixel.Color(0, 0, 0));   // Off
            pixel.show();
            delay(500);
        }

    // private:
        
};

#endif