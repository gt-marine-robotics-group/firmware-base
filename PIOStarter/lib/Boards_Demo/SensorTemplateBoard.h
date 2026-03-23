#pragma once

/**
 * @file SensorBoard.h
 * @author Jason Hsiao
 * @date 12/24/2025
 * @version 1.0
 *
 * @brief Header (and implementation) for Sensor board.
 *
 * This file controls the logical flow of the board and the 
 * instantiation and function calls of different modules.
 * @see config.h for the hardware definitions like GPIO mapping 
 * and platform.ini for build flag definitions.
 */

 
#ifdef SENSOR_DEMO

#include "config.h"
// #include "LEDPIO.h"
// #include "Estop.h"
// #include "TempSensor.h"
#include "DOFStick.h"
// #include "LEDMux.h"
#include "ProtoSender.h"

/**
 * @brief 
 * Acts as centralized logical flow for the application.
 * This class is where we actually implement logic and use our modules. 
 * @note This implementation is an alternative to the current  
 * AppCore approach because I find the number of build flags 
 * unsightly.
 * @note Is the equivalent of the main.cpp in most applications, 
 * but is abstracted for modularity and customizability.
 */
class SensorTemplateBoard {
public:
    /**
     * @brief Set-up all of our modules and the Serial monitor.
     * @note Initializes all the modules just like setup() in Arduino
     * @todo Maybe consider a name change then lol
     */
    void begin() {
        // Serial.begin(115200);
        
        // // Only initializes what exists for this specific board
        sensor.setup();
        protoSender.setup();
    }

    /**
     * @brief Main loop for all of our modules.
     * @note Do all of your logic here just like loop() in Arduino.
     * @todo Okay might actually need to change these names
     */
    void update() {
        // CPU doesn't need to worry about the PIO running in the background
        SensorData_t sensorData = sensor.readData();
        Envelope env = Envelope_init_zero;
        // // Print Accelerometer Data (in m/s^2)
        // Serial.println("Accelerometer Data (m/s^2):");
        // Serial.print("X: "); Serial.print(sensorData.data.accel.x); 
        // Serial.print(" Y: "); Serial.print(sensorData.data.accel.y); 
        // Serial.print(" Z: "); Serial.println(sensorData.data.accel.z);

        env.which_payload = Envelope_readings_tag;
        env.payload.readings.id = (uint32_t)sensorData.type;

        // Handling the Union/OneOf correctly
        if (sensorData.type == SensorData_t::IMU) {
            env.payload.readings.which_data = sensorData_accel_tag;
            env.payload.readings.data.accel.x = sensorData.data.accel.x;
            env.payload.readings.data.accel.y = sensorData.data.accel.y;
            env.payload.readings.data.accel.z = sensorData.data.accel.z;
        } else if (sensorData.type == SensorData_t::TEMP) {
            env.payload.readings.which_data = sensorData_temperature_tag;
            env.payload.readings.data.temperature = sensorData.data.temperature;
        }
        protoSender.sendData(env);
        delay(1000);
        
        if (cycle == 0) {
            message = message1;
        } else if (cycle == 1) {
            message = message2;
        } else {
            message = message3;
        }
        cycle = (cycle + 1) % 3;
        env = Envelope_init_zero;
        env.which_payload = Envelope_debug_tag;
        env.payload.debug.id = 86 + cycle;
        strncpy(env.payload.debug.content, message, sizeof(env.payload.debug.content) - 1);
        protoSender.sendData(env);
        delay(1000);
        
        // Serial.println("I am alive");
        // delay(1000);
    }

private:
    // These objects are only "born" if the build flag is active
    DOFStick sensor;
    ProtoSender protoSender;

    char* message;

    // Test messages
    uint8_t cycle = 0;
    char message1[64] = "These are not the drones you're looking for";
    char message2[64] = "Understandable have a nice day";
    char message3[64] = "Wait a min this stuff actually works";
};

#endif