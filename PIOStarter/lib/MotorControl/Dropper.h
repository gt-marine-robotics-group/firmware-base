#pragma once

/**
 * @file Dropper.h
 * @author Jason Hsiao
 * @date 4/8/2025
 * @version 1.0
 *
 * @brief Header for Dropper Controller.
 *
 * This file allows you to initialize and control PWM outputs,
 * essentially acting as a dropper controller.
 * @see config.h for the hardware definitions like GPIO mapping
 */

#ifdef HAS_DROPPER

#include <Arduino.h>
#include <107-Arduino-Servo-RP2040.h>
#include "config.h"

using namespace _107_;

/**
 * @brief Controls the physical dropper.
 * * This class manages PWM generation and direction pins. It is only
 * compiled if the HAS_DROPPER flag is defined in the build environment.
 */
class Dropper{
    public:
        /**
         * @brief Construct a new Dropper object.
         * @note Does not initialize hardware. Use setup() for GPIO/PIO setup 
         * to ensure the hardware is ready after the system clock stabilizes.
         */
        Dropper();

        /**
         * @brief Set-up the new Dropper object.
         * @note Initializes hardware.
         * @see config.h for the GPIO mapping.
         */
        bool setup();

        /**
         * @brief Emergency stop for the Dropper object. 
         * Turns off all the motors
         * @return Whether the e-stop attempt was successful.
         */
        bool estop();

        /**
         * @brief Sets the motor speed and direction.
         * @param PWM The PWM duty cycle (0-255).
         * @param pin GPIO pin you desire to write to
         * @return The actual PWM value applied after safety clamping.
         */
        void spinMotor(uint8_t mode);

        void sweepMotors();
        
    private:
        Servo dropper;
};

#endif