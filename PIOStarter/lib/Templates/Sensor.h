#pragma once

/**
 * @file Sensor.h
 * @author Jason Hsiao
 * @date 3/11/2025
 * @version 1.0
 *
 * @brief Header for Sensor Parent Class.
 *
 * This file is the base template class for implementing sensors.
 * @see config.h for the hardware definitions like GPIO mapping.
 */

#ifdef HAS_SENSOR

#include <Arduino.h>
#include "config.h"
#include <Wire.h>

// #include <SparkFunLSM9DS1.h>

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!


/**
 * @brief 
 * Acts as a wrapper for the Adafruit sensor library.
 * This class manages initializing and updating the sensor object. 
 * @note Only compiled if the HAS_SENSORS flag is defined in the build environment.
 * Also is tied to the same I2C bus as the TempSensor object.
 */
class Sensor{
    public:
        /**
         * @brief Construct a new Sensor object.
         * @note Does not initialize hardware. Use setup() for GPIO/PIO setup 
         * to ensure the hardware is ready after the system clock stabilizes.
         */
        Sensor();

        /**
         * @brief Set-up the new Sensor object.
         * @note Initializes hardware.
         */
        void setup();

        /**
         * @brief Emergency stop for the Sensor object.
         * @return Whether the e-stop attempt was successful.
         * @note Not implemented due to passive nature of sensors
         */
        bool estop();

        /**
         * @brief Read and print the current sensor data
         * @return the current sensor data (currently just a placeholder)
         * @todo Make a struct for the sensor data or just directly load the protobuf envelope
         * @see included library for information on sensor operation and usage
         */
        uint16_t readData();

    private:
        /// LSM9DS1 imu object, created here so no other class can mess with it (OOP concept)
        Adafruit_LSM9DS1 imu;
};

#endif