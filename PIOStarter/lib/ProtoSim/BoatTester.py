import serial
from cobs import cobs
import message_pb2
import threading
import time
import sys

# --- Configuration ---
SERIAL_PORT = 'COM26'  # Update to your Pico's port
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
except Exception as e:
    print(f"Error: Could not open {SERIAL_PORT}. {e}")
    sys.exit(1)

def listener():
    """Thread function to constantly listen for E-Stop status from Pico."""
    buffer = bytearray()
    while True:
        if ser.in_waiting > 0:
            char = ser.read(1)
            if char == b'\x00':
                if buffer:
                    try:
                        # Unstuff and Parse
                        decoded = cobs.decode(buffer)
                        envelope = message_pb2.Envelope()
                        envelope.ParseFromString(decoded)
                        
                        # Check if it's a status message
                        if envelope.WhichOneof("payload") == "status":
                            estop_state = envelope.status.estop
                            manual_state = envelope.status.manual
                            print(f"\n[PICO STATUS] E-Stop: {'LOCKED' if estop_state else 'Clear'}, Manual Mode: {manual_state}")
                            print("Enter '1' for Manual ON, '0' for Manual OFF: ", end="", flush=True)
                    except Exception as e:
                        pass # Ignore malformed packets
                    buffer = bytearray()
            else:
                buffer.extend(char)
        time.sleep(0.01)

def send_manual_mode(is_manual):
    """Sends a status message to the Pico setting the manual bit."""
    status_msg = message_pb2.operationStatus()
    status_msg.manual = is_manual
    status_msg.estop = False # Laptop usually doesn't trigger hardware estop

    envelope = message_pb2.Envelope()
    envelope.status.CopyFrom(status_msg)

    serialized = envelope.SerializeToString()
    encoded = cobs.encode(serialized)
    ser.write(encoded + b'\x00')

# Start the listener thread
t = threading.Thread(target=listener, daemon=True)
t.start()

print("--- Marine Robotics Command Center ---")
print("Listening for Pico E-Stop status...")

try:
    while True:
        cmd = input("Enter '1' for Manual ON, '0' for Manual OFF: ")
        if cmd == '1':
            send_manual_mode(True)
        elif cmd == '0':
            send_manual_mode(False)
        elif cmd.lower() == 'q':
            break
except KeyboardInterrupt:
    pass
finally:
    ser.close()