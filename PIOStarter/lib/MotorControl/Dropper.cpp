/**
 * @file Dropper.cpp
 * @author Jason Hsiao
 * @date 4/8/2025
 * @version 1.0
 *
 * @brief Implementation of Dropper.
 *
 * This file allows you to initialize and control PWM outputs,
 * essentially acting as a dropper controller.
 * @see config.h for the hardware definitions like GPIO mapping
 */

 #ifdef HAS_DROPPER

#include "Dropper.h"


// Constructor
Dropper::Dropper() {
    // Initialize internal variables if needed

}

// TODO: instantiate variable amounts of PWM outputs (defined in config.h)
bool Dropper::setup(){
    bool motorsAttached = true;
    dropper.attach(9);
    return motorsAttached;
}

bool Dropper::estop(){
    spinMotor(0);
    return true;
}

void Dropper::spinMotor(uint8_t mode) {
    switch (mode) {
    case 0:
        dropper.writeMicroseconds(1500);
        break; 
    case 1:
        dropper.writeMicroseconds(1000);
        break; 
    case 2:
        dropper.writeMicroseconds(2000);
        break;
    default:
        dropper.writeMicroseconds(1500);
    }
}

// Just sweeping the motors for testing purpose, can make elegant code later
void Dropper::sweepMotors() {
    for (int i = 1000; i < 2000; i++){
        dropper.writeMicroseconds(i);
        delay(1);
    }
    
    for (int i = 2000; i > 1000; i--){
        // Serial.println(i);
        dropper.writeMicroseconds(i);
        delay(1);
    }
}

#endif
