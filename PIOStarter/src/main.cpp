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