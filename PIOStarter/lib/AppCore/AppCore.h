// lib/AppCore/AppCore.h
#pragma once

#include "config.h"
#include "MotorController.h"
#include "LEDPIO.h"
#include "Estop.h"

class AppCore {
public:
    void begin() {
        Serial.begin(115200);
        
        // Only initializes what exists for this specific board
        #ifdef HAS_ESTOP // Might need to fix this later
            motor.setup();
            estop.setup();
        #endif
        
        #ifdef HAS_INDICATOR_LED
            ledPIO.setup();
        #endif

        #ifdef HAS_SENSORS
            sensors.begin();
        #endif
    }

    void update() {
        // CPU doesn't need to worry about the PIO running in the background
        #ifdef HAS_ESTOP // Might need to fix this later
            if (!Estop::estopTriggered){
                // uint16_t pot = analogRead(POT_IN);
                uint16_t pwm = motor.spinMotor(config::PWM_OUT, analogRead(config::POT_IN));
                ledPIO.updateBlink(pwm);
            } else {
                motor.estop();
            }
        #endif
        
    }

private:
    // These objects are only "born" if the build flag is active
    #ifdef HAS_MOTORS && HAS_ESTOP
        MotorController motor; 
        Estop estop;
    #endif

    #ifdef HAS_INDICATOR_LED
        LEDPIO ledPIO;
    #endif
    
    #ifdef HAS_SENSORS
        SensorPackage sensors;
    #endif
};