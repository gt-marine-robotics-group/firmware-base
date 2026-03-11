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

/**
 * @brief 
 * Acts as a boilerplate struct for potential sensor data
 * @note Only compiled if the HAS_SENSOR flag is defined in the build environment.
 */
struct SensorData_t {
    enum Type { TEMP, IMU, CURRENT } type;
    union {
        float temperature;
        struct { float x, y, z; } accel;
        float amperage;
    } data;
};

/**
 * @brief 
 * Acts as a template class for all future implementations of sensors
 * This class represents how to initialize and update the sensor object. 
 * @note Only compiled if the HAS_SENSOR flag is defined in the build environment.
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
         * @brief Destructor for Sensor object.
         * @note Likely will not use because this system is not dynamic
         */
        virtual ~Sensor() {} // Critical for safe cleanup

        /**
         * @brief Set-up the new Sensor object.
         * @note Initializes hardware.
         */
        virtual void setup() = 0;

        /**
         * @brief Emergency stop for the Sensor object.
         * @return Whether the e-stop attempt was successful.
         * @note Not implemented due to passive nature of sensors
         */
        virtual bool estop() = 0;

        /**
         * @brief Read and print the current sensor data
         * @return the current sensor data (currently just a placeholder)
         * @todo Make a struct for the sensor data or just directly load the protobuf envelope
         * @see included library for information on sensor operation and usage
         */
        virtual uint16_t readData() = 0;

};

#endif