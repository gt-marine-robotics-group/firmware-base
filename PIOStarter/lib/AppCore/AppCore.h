// lib/AppCore/AppCore.h
#pragma once

#include "config.h"
#include "MotorController.h"
#include "LEDPIO.h"
#include "Estop.h"
#include "TempSensor.h"
#include "DOFStick.h"

class AppCore {
public:
    void begin() {
        Serial.begin(115200);
        
        // Only initializes what exists for this specific board
        #ifdef HAS_ESTOP // Might need to fix this later
            estop.setup();
        #endif

        #ifdef HAS_MOTORS
            motor.setup();
        #endif

        #ifdef HAS_INDICATOR_LED
            ledPIO.setup();
        #endif

        #ifdef HAS_SENSORS
            tempSensor.setup();
            imu.setup();
        #endif
    }

    void update() {
        // CPU doesn't need to worry about the PIO running in the background
        #ifdef HAS_ESTOP // Might need to fix this later, but I think motors are the only thing E-stopped
            #ifdef HAS_MOTORS // if there is not an EStop consider using #else if (true) {
            if (!Estop::estopTriggered){
                pwm = motor.spinMotor(config::PWM_OUT, analogRead(config::POT_IN));
            } else {
                motor.estop();
            }
            #endif
        #endif

        #ifdef HAS_SENSORS
            temp = tempSensor.readData();
            imu.readData();
            Serial.println("\n\n");
            // Serial.println(temp);
        #endif
        
        // Transfer the sensor or output value to LED blink
        #ifdef HAS_INDICATOR_LED
            #ifdef HAS_MOTORS 
                pwm_input = pwm;
            #endif
            #ifdef HAS_SENSORS      // Check to see if #elifdef is supported
                pwm_input = temp;
            #endif
        #endif

        #ifdef HAS_INDICATOR_LED
            // Feed the PIO the new blinking value
            ledPIO.updateBlink(pwm_input);
        #endif

        delay(1000);
    }

private:
    // These objects are only "born" if the build flag is active
    #ifdef HAS_MOTORS
        MotorController motor; 
        uint16_t pwm;
    #endif

    #ifdef HAS_ESTOP
        Estop estop;
    #endif

    #ifdef HAS_INDICATOR_LED
        LEDPIO ledPIO;
        uint8_t pwm_input = 25;
    #endif
    
    #ifdef HAS_SENSORS
        TempSensor tempSensor;
        uint8_t temp = 25;
        DOFStick imu;
    #endif
};