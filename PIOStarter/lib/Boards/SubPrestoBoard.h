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

#ifdef BOARD_EPSILON

#include "config.h"
#include "ProtoSender.h"
#include "ProtoReceiver.h"
#include "LEDMux.h"
#include "Estop.h"
#include "MotorController.h"


/**
 * @brief 
 * Acts as centralized logical flow for the PrestoJrBoard specifically.
 * This class is where we actually implement logic and use our modules. 
 * @note Is the equivalent of the main.cpp in most applications, but is abstracted for modularity 
 * and customizability.
 */
class SubPrestoBoard {
public:
    /**
     * @brief Set-up all of our modules and the Serial monitor.
     * @note Initializes all the modules just like setup() in Arduino
     * @todo Maybe consider a name change then lol
     */
    void begin() {
        // Serial.begin(9600);
        // Serial.println("Initialized :)");

        // Only initializes what exists for this specific board
        protoSender.setup();        // Check if these are blocking because the code doesn't start until it connects
        protoReceiver.setup();      // Not that that is a bad thing, it's just packetSerial.begin() is a blocking call
        
        estop.setup();
        ledMux.setup();
        
        uint32_t seq = (0b0100);
        if (motorController.setup()){
            seq = 0b0110;
        }
        ledMux.updateLEDSequence(seq);
        // pinMode(16, INPUT_PULLDOWN);
        
    }

    /**
     * @brief Main loop for all of our modules.
     * @note Do all of your logic here just like loop() in Arduino.
     * @todo Okay might actually need to change these names
     */
    void update() {
        if (Estop::estopTriggered) {
            motorController.estop();
            protoSender.sendStatus(true, false); 
            uint32_t estopped = (0b1000);
            ledMux.updateLEDSequence(estopped);
        } else {
            // protoSender.sendStatus(false, true);
            
            // motorController.sweepMotors(); // Initial Dry Test, comment this out during the actual motor run

            protoReceiver.receiveData();

            if (ProtoReceiver::newMessage){
                uint32_t message = (0b010);
                ledMux.updateLEDSequence(message);
                motorController.spinMotors(ProtoReceiver::motor_commands);
                // delay(1000);
                
                uint32_t seq = (0b0100);
                ledMux.updateLEDSequence(seq);

                ProtoReceiver::newMessage = false;
            }
        }
        
        // delay(1000);

        }


private:
    // Unlike AppCore's approach, we assume these objects exist due the platform.ini definitions
    ProtoSender protoSender;
    ProtoReceiver protoReceiver;
    MotorController motorController;
    Estop estop;
    LEDMux ledMux;
};

#endif