#pragma once

#include <Arduino.h>
#include "config.h"


class Estop{
    public:
        static inline volatile bool estopTriggered = false;
        
        static void handleEstop() {
            Serial.println("E-stop was triggered");
            estopTriggered = true;
            
            digitalWrite(config::ESTOP_LED, HIGH);
        }

        void setup(){
            // Ensure the pin is configured correctly before attaching the interrupt
            pinMode(config::ESTOP_PIN, INPUT_PULLDOWN); 
            
            // Attach the interrupt to the pin
            // Syntax: attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)
            attachInterrupt(digitalPinToInterrupt(config::ESTOP_PIN), handleEstop, FALLING);
            
            pinMode(config::ESTOP_LED, OUTPUT); 
            digitalWrite(config::ESTOP_LED, LOW);

        }

    private:
};