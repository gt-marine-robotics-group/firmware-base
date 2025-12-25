/**
 * @file main.cpp
 * @author Jason Hsiao
 * @date 12/24/2025
 * @version 1.0
 * 
 * @brief this just starts the AppCore object, but see the below for documentation syntax.
 * 
 * @brief This is a brief description of the file's purpose.
 *
 * This section provides a more detailed description of what the file does,
 * its contents, and any dependencies or usage instructions.
 */

#include <Arduino.h>
#include "hardware/pio.h"
#include "config.h"
#include "AppCore.h"

// This is just the main test, but can eventually be something like presto for clarity
AppCore myApp;

// Don't do anything here, try and configure in AppCore for maximum flexibility and cleanliness
void setup() {
    myApp.begin();
}

void loop() {
    myApp.update();
}