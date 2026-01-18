/**
 * @file ProtoSender.cpp
 * @author Jason Hsiao
 * @date 1/16/2025
 * @version 1.0
 *
 * @brief Implementation of Protobuf sender.
 *
 * This file holds the functions for instantiating, setting up, 
 * and sending data over ProtoBuf.
 * 
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
        // Serial.println("Failed to encode!!");
    }
    myPacketSerial.send(buffer, stream.bytes_written);
    return 0;
}

#endif