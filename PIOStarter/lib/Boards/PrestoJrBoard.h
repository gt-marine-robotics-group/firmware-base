#pragma once

/**
 * @file PrestoJrBoard.h
 * @author Jason Hsiao
 * @date 12/24/2025
 * @version 1.0
 *
 * @brief Header (and implementation) for Presto board.
 *
 * This file controls the logical flow of the board and the 
 * instantiation and function calls of different modules.
 * @note This implementation is an alternative to the current  
 * AppCore approach because I find the number of build flags 
 * unsightly.
 * @see config.h for the hardware definitions like GPIO mapping 
 * and platform.ini for build flag definitions.
 */

#ifdef BOARD_GAMMA

#include "config.h"
#include "ProtoSender.h"

/**
 * @brief 
 * Acts as centralized logical flow for the PrestoJrBoard specifically.
 * This class is where we actually implement logic and use our modules. 
 * @note Is the equivalent of the main.cpp in most applications, but is abstracted for modularity 
 * and customizability.
 */
class PrestoJrBoard {
public:
    /**
     * @brief Set-up all of our modules and the Serial monitor.
     * @note Initializes all the modules just like setup() in Arduino
     * @todo Maybe consider a name change then lol
     */
    void begin() {
        // Serial.begin(115200);
        // Only initializes what exists for this specific board
        protoSender.setup();
    }

    /**
     * @brief Main loop for all of our modules.
     * @note Do all of your logic here just like loop() in Arduino.
     * @todo Okay might actually need to change these names
     */
    void update() {
        // CPU doesn't need to worry about the PIO running in the background
        protoSender.sendData();        
        delay(1000);
    }

private:
    // Unlike AppCore's approach, we assume these objects exist due the platform.ini definitions
    ProtoSender protoSender;
};

#endif