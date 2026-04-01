/**
 * @file DOFStick.cpp
 * @author Jason Hsiao
 * @date 12/24/2025
 * @version 1.0
 *
 * @brief Implementation of 9 DOF Stick Sensor Interface.
 *
 * This file holds the functions for instantiating, setting up, 
 * and reading/printing the sensor data.
 * 
 * @todo See if there is a way to directly interface with Protobuf or PIO DMA
 * @see config.h for the hardware definitions like GPIO mapping
 */

#ifdef HAS_DOF_SENSOR

#include "DOFStick.h"

// Constructor
DOFStick::DOFStick() {
    // Initialize internal variables if needed

}

void DOFStick::setup(){
    // Set up the I2C object - Unnecessary since already defined in Temp Sensor
    Wire1.setSDA(config::TEMP_SDA);
    Wire1.setSCL(config::TEMP_SCL);
    Wire1.begin();

    // // Try this solution later:
    // // See if ANY device is at the IMU's address (0x6B or 0x1E)
    // // endTransmission returns 0 only if it gets an ACK from hardware
    // Wire1.beginTransmission(0x6B); 
    // if (Wire1.endTransmission() != 0) {
    //     Serial.println("I2C Hardware Error: No device ACK at 0x6B. Skipping IMU.");
    //     imu_active = false; // Use a flag to skip imu.read() in your loop
    // } else {
    //     imu = Adafruit_LSM9DS1(&Wire1, 0);
    //     if (!imu.begin()) {
    //         Serial.println("IMU found but failed to initialize. Check power?");
    //         imu_active = false;
    //     } else {
    //         imu_active = true;
    //         Serial.println("IMU Online!");
    //     }
    // }
    // Initialize the sensor by passing the Wire1 pointer
    imu = Adafruit_LSM9DS1(&Wire1, 0);
    if (!imu.begin()) {
        Serial.println("9DOF Sensor not detected. Please check connections.");
    }

    // Set the accelerometer range
    imu.setupAccel(imu.LSM9DS1_ACCELRANGE_2G, imu.LSM9DS1_ACCELDATARATE_10HZ);
    //imu.setupAccel(imu.LSM9DS1_ACCELRANGE_4G, imu.LSM9DS1_ACCELDATARATE_119HZ);
    //imu.setupAccel(imu.LSM9DS1_ACCELRANGE_8G, imu.LSM9DS1_ACCELDATARATE_476HZ);
    //imu.setupAccel(imu.LSM9DS1_ACCELRANGE_16G, imu.LSM9DS1_ACCELDATARATE_952HZ);
    
    // 2.) Set the magnetometer sensitivity
    imu.setupMag(imu.LSM9DS1_MAGGAIN_4GAUSS);
    //imu.setupMag(imu.LSM9DS1_MAGGAIN_8GAUSS);
    //imu.setupMag(imu.LSM9DS1_MAGGAIN_12GAUSS);
    //imu.setupMag(imu.LSM9DS1_MAGGAIN_16GAUSS);

    // 3.) Setup the gyroscope
    imu.setupGyro(imu.LSM9DS1_GYROSCALE_245DPS);
    //imu.setupGyro(imu.LSM9DS1_GYROSCALE_500DPS);
    //imu.setupGyro(imu.LSM9DS1_GYROSCALE_2000DPS);
    // Serial.println("9DOF Sensor Initialized Successfully.");

}

bool DOFStick::estop(){
    // No need to estop a passive sensor
    // but can just end the I2C connection
    return true;
}

// If syntax here looks weird, my bad, I took some code from 4180 and vibed it to work
SensorData_t DOFStick::readData() {
    // Read acceleration, gyroscope, and magnetometer data
    imu.read();

    // Variables to store sensor readings
    sensors_event_t accel, gyro, mag, temp;

    // Read accelerometer data
    imu.getEvent(&accel, &gyro, &mag, &temp);

    lastReading.type = SensorData_t::IMU;
    lastReading.data.accel.x = accel.acceleration.x;
    lastReading.data.accel.y = accel.acceleration.y;
    lastReading.data.accel.z = accel.acceleration.z;

    // // Print Accelerometer Data (in m/s^2)
    // Serial.println("Accelerometer Data (m/s^2):");
    // Serial.print("X: "); Serial.print(accel.acceleration.x); 
    // Serial.print(" Y: "); Serial.print(accel.acceleration.y); 
    // Serial.print(" Z: "); Serial.println(accel.acceleration.z);

    // // Print Gyroscope Data (in rad/s)
    // Serial.println("Gyroscope Data (rad/s):");
    // Serial.print("X: "); Serial.print(gyro.gyro.x); 
    // Serial.print(" Y: "); Serial.print(gyro.gyro.y); 
    // Serial.print(" Z: "); Serial.println(gyro.gyro.z);

    // // Print Magnetometer Data (in uTesla)
    // Serial.println("Magnetometer Data (uT):");
    // Serial.print("X: "); Serial.print(mag.magnetic.x); 
    // Serial.print(" Y: "); Serial.print(mag.magnetic.y); 
    // Serial.print(" Z: "); Serial.println(mag.magnetic.z);

    return lastReading;
}

// void DOFStick::loadData(Envelope* envelope) {
//     imu.read();

//     // Variables to store sensor readings
//     sensors_event_t accel, gyro, mag, temp;

//     // Read accelerometer data
//     imu.getEvent(&accel, &gyro, &mag, &temp);

//     Envelope env = *envelope;
//     // Set the tag so the receiver knows which one it is
//     env.which_payload = Envelope_readings_tag;
//     env.payload.readings.id = 1;

//     // env.payload.readings.data[1] = accel.acceleration.x;
//     // env.payload.readings.data[2] = accel.acceleration.y;
//     // env.payload.readings.data[3] = accel.acceleration.z;
    
//     // Print Accelerometer Data (in m/s^2)
//     Serial.println("Accelerometer Data (m/s^2):");
//     Serial.print("X: "); Serial.print(accel.acceleration.x); 
//     Serial.print(" Y: "); Serial.print(accel.acceleration.y); 
//     Serial.print(" Z: "); Serial.println(accel.acceleration.z);

//     // Print Gyroscope Data (in rad/s)
//     Serial.println("Gyroscope Data (rad/s):");
//     Serial.print("X: "); Serial.print(gyro.gyro.x); 
//     Serial.print(" Y: "); Serial.print(gyro.gyro.y); 
//     Serial.print(" Z: "); Serial.println(gyro.gyro.z);

//     // Print Magnetometer Data (in uTesla)
//     Serial.println("Magnetometer Data (uT):");
//     Serial.print("X: "); Serial.print(mag.magnetic.x); 
//     Serial.print(" Y: "); Serial.print(mag.magnetic.y); 
//     Serial.print(" Z: "); Serial.println(mag.magnetic.z);
// }


// void DOFStick::loadData(Envelope* envelope) {
//     return;
// }


#endif

