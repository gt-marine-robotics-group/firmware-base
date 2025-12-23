#include "MotorController.h"

// Constructor
MotorController::MotorController() {
    // Initialize internal variables if needed

}

void MotorController::setup(){
    pinMode(config::PWM_OUT, OUTPUT);
    pinMode(config::AI1, OUTPUT);
    pinMode(config::AI2, OUTPUT);
}

bool MotorController::estop(){
    digitalWrite(config::AI1, LOW);
    digitalWrite(config::AI2, LOW);
    analogWrite(config::PWM_OUT, 0);
    return true;
}

uint16_t MotorController::spinMotor(uint8_t pin, int pot) {
    int32_t delta = static_cast<int32_t>(pot) - ADC_CENTER;

    // Deadband → stop
    if (abs(delta) < DEADBAND) {
        digitalWrite(config::AI1, LOW);
        digitalWrite(config::AI2, LOW);
        analogWrite(config::PWM_OUT, 0);
        return 0;
    } else {        
        // Direction
        bool forward = (delta > 0);

        // Scale magnitude: |delta| → 0–255
        uint8_t pwm = (static_cast<uint32_t>(abs(delta)) * PWM_MAX) / ADC_CENTER;

        // Safety clamp
        if (pwm > PWM_MAX) pwm = PWM_MAX;

        // Drive H-bridge
        digitalWrite(config::AI1, forward ? HIGH : LOW);
        digitalWrite(config::AI2, forward ? LOW  : HIGH);
        analogWrite(config::PWM_OUT, pwm);
        return pwm;
    }
    return 0;
}

void MotorController::spinMotors(uint8_t* pins, int* PWMs, uint16_t* results, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        results[i] = spinMotor(pins[i], PWMs[i]);
    }
}

