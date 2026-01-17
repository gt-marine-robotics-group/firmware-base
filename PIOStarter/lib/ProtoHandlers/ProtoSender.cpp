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

#ifdef HAS_PROTO

#include "protoSender.h"

// Constructor
ProtoSender::ProtoSender() {
    // Initialize internal variables if needed

}

void ProtoSender::setup(){
    myPacketSerial.begin(115200);

    msg = MyMessage_init_zero;
    
    // Create a stream that writes to our buffer
    stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    
}

uint16_t ProtoSender::sendData(){
    msg.id = 42;
    strcpy(msg.content, "Hello World");

    // Encode the message
    if (!pb_encode(&stream, MyMessage_fields, &msg)) {
        // Handle error
    }
    myPacketSerial.send(buffer, stream.bytes_written);
    return 0;
}

#endif