#ifdef HAS_SENSORS

#include "I2CSensor.h"

// Constructor
I2CSensor::I2CSensor() {
    // Initialize internal variables if needed

}

void I2CSensor::setup(){
    // Set up the I2C object
    Wire1.setSDA(config::TEMP_SDA);
    Wire1.setSCL(config::TEMP_SCL);
    Wire1.begin();


    // 2. Initialize the sensor by passing the Wire1 pointer
    while (!sensor.begin()) {
        Serial.println("Could not find a valid Si7021 sensor, check wiring!");
    }
    Serial.println("Si7021 initialized!");

}

bool I2CSensor::estop(){
    // No need to estop a passive sensor
    // but can just end the I2C connection
    return true;
}

// If syntax here looks weird, my bad, I took some code from 4180 and vibed it to work
uint16_t I2CSensor::readData() {
    Serial.print("Humidity: ");
    Serial.print(sensor.readHumidity(), 2);
    Serial.print("%\tTemperature: ");
    Serial.print(sensor.readTemperature(), 2);
    Serial.println("C");

    delay(1000);
    return 1;
}

#endif

