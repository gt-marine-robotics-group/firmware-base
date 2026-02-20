import serial
from cobs import cobs
import message_pb2
import threading
import time
import sys

# --- Configuration ---
# SERIAL_PORT = 'COM26'  # Update to your Pico's port
BAUD_RATE = 115200

# ... (imports) ...
import serial.tools.list_ports

def find_pico_port():
    """Search for a Raspberry Pi Pico by Hardware ID or Name."""
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        # Check by Name
        if "Pico" in port.description or "RP2" in port.description:
            return port.device
        
        # Check by Hardware ID (VID:PID for Pi Pico is usually 2E8A:0005)
        if port.vid == 0x2E8A:
            return port.device
            
    return None
# 1. Detect the port
detected_port = find_pico_port()

if detected_port:
    print(f"Detected Pico on {detected_port}")
    try:
        ser = serial.Serial(detected_port, 115200, timeout=0.1)
    except Exception as e:
        print(f"Could not open {detected_port}: {e}")
        sys.exit(1)
else:
    print("Error: Raspberry Pi Pico not found! Is it plugged in?")
    # Optional: List available ports to help debugging
    print("Available ports:")
    for p in serial.tools.list_ports.comports():
        print(f" - {p.device}: {p.description}")
    sys.exit(1)

# ... (rest of your threading and logic) ...

# try:
#     ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
# except Exception as e:
#     print(f"Error: Could not open {SERIAL_PORT}. {e}")
#     sys.exit(1)

def listener():
    """Thread function optimized for high-speed serial processing."""
    buffer = bytearray()
    while True:
        try:
            # 1. Read everything currently in the OS serial buffer
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                buffer.extend(data)

                # 2. Check if we have at least one complete packet (0x00)
                while b'\x00' in buffer:
                    # Split at the first 0x00 found
                    packet, buffer = buffer.split(b'\x00', 1)

                    if packet:
                        try:
                            # 3. Unstuff and Parse
                            decoded = cobs.decode(packet)
                            envelope = message_pb2.Envelope()
                            envelope.ParseFromString(decoded)
                            
                            if envelope.WhichOneof("payload") == "status":
                                estop = envelope.status.estop
                                manual = envelope.status.manual
                                # Use sys.stdout.write for faster UI updates than print()
                                sys.stdout.write(f"\r[STATUS] E-Stop: {estop} | Manual: {manual}   ")
                                sys.stdout.flush()

                        except Exception as e:
                            # Likely a partial or corrupted packet; skip it
                            pass
            
            # 4. Tiny sleep to prevent 100% CPU usage, but keep it responsive
            time.sleep(0.001) 
            
        except Exception as e:
            print(f"\nRead Error: {e}")
            break

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