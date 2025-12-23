#include <Arduino.h>
#include "hardware/pio.h"
#include "config.h"
#include "AppCore.h"


AppCore myApp;

void setup() {
    myApp.begin();
}

void loop() {
    myApp.update();
}