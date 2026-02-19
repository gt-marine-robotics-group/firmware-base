#pragma once

/**
 * @file PrestoJrBoard.h
 * @author Jason Hsiao
 * @date 1/17/2025
 * @version 1.0
 *
 * @brief Header (and implementation) for Presto Jr board.
 *
 * This file controls the logical flow of the board and the 
 * instantiation and function calls of different modules.
 * @note This implementation is an alternative to the current  
 * AppCore approach because I find the number of build flags 
 * unsightly.
 * @see config.h for the hardware definitions like GPIO mapping 
 * and platform.ini for build flag definitions.
 */

#ifdef BOARD_DELTA

#include "config.h"
#include "ProtoSender.h"
#include "ProtoReceiver.h"
#include "LEDMux.h"
#include "Estop.h"

/**
 * @brief 
 * Acts as centralized logical flow for the PrestoJrBoard specifically.
 * This class is where we actually implement logic and use our modules. 
 * @note Is the equivalent of the main.cpp in most applications, but is abstracted for modularity 
 * and customizability.
 */
class ProtoBoard {
public:
    /**
     * @brief Set-up all of our modules and the Serial monitor.
     * @note Initializes all the modules just like setup() in Arduino
     * @todo Maybe consider a name change then lol
     */
    void begin() {
        // Serial.begin(115200);

        // Only initializes what exists for this specific board
        protoSender.setup();        // Check if these are blocking because the code doesn't start until it connects
        protoReceiver.setup();      // Not that that is a bad thing
        // motorController.setup();
        estop.setup();
        // ledMux.setup();

        // uint32_t seq = (0b1000 << 4) + (0b0100);
        // uint32_t seq = (0b0000);
        // ledMux.updateLEDSequence(seq);
        pinMode(19, OUTPUT);
        pinMode(20, OUTPUT);
        pinMode(21, OUTPUT);
        pinMode(22, OUTPUT);

    }

    /**
     * @brief Main loop for all of our modules.
     * @note Do all of your logic here just like loop() in Arduino.
     * @todo Okay might actually need to change these names
     */
    void update() {
        // CPU doesn't need to worry about the PIO running in the background
        // if (Estop::estopTriggered) {
        //     protoSender.sendStatus(true, false); 
        //     // uint32_t estopped = (0b1000);
        //     // ledMux.updateLEDSequence(estopped);
        // } else {
        //     // uint32_t estopped = (0b0100);
        //     // ledMux.updateLEDSequence(estopped);
        //     protoSender.sendStatus(false, true);
        // }
        
        protoSender.sendStatus(Estop::estopTriggered, false); 
        digitalWrite(19, Estop::estopTriggered);
        digitalWrite(20, Estop::estopTriggered);
        digitalWrite(21, (Estop::estopTriggered) ? HIGH : !ProtoReceiver::manual_mode);    // Whenever e-stop is triggered, write high to turn it off, and when it's false, write !manual to turn it on
        digitalWrite(22, !Estop::estopTriggered);
        protoReceiver.receiveData();
        
        if (ProtoReceiver::newMessage){
            // Put whatever new message handling you want here, this is just blinking the LED for a second, but this is blocking so don't do this

            // uint32_t message = (0b010);
            // ledMux.updateLEDSequence(message);
            // // motorController.spinMotors(ProtoReceiver::motor_commands);
            // // delay(1000);
                
            // uint32_t seq = (0b0100);
            // ledMux.updateLEDSequence(seq);

            ProtoReceiver::newMessage = false;
        }
        
        delay(10);

        }


private:
    // Unlike AppCore's approach, we assume these objects exist due the platform.ini definitions
    ProtoSender protoSender;
    ProtoReceiver protoReceiver;
    // MotorController motorController;
    Estop estop;
    // LEDMux ledMux;
    // config::Color color = config::Color::Off;
};

#endif