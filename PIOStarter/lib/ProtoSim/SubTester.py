import serial
import serial.tools.list_ports
from cobs import cobs
import message_pb2
import threading
import time
import sys
import math
from blessed import Terminal

# --- 1. Serial & Discovery Logic ---
def find_pico_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if port.vid == 0x2E8A or "Pico" in port.description:
            return port.device
    return None

detected_port = find_pico_port()
if not detected_port:
    print("Error: Raspberry Pi Pico not found!")
    sys.exit(1)

ser = serial.Serial(detected_port, 115200, timeout=0.01)
term = Terminal()

# --- 2. Global State ---
motors = [1500] * 8  
active_motors = [1500] * 8  
selected_idx = 0
group_mode = False
live_mode = False
sine_mode = False
sine_offset = 0.0

last_send_time = 0
MIN_SEND_INTERVAL = 0.02  # 50Hz cap (safety for manual bursts)
HEARTBEAT_INTERVAL = 0.1   # 100ms (Keeps firmware watchdog happy)

pico_status = {"estop": False, "manual": False, "debug": "No messages yet"}

# --- 3. Communication Logic ---
def send_motors(values):
    """Encodes and sends the current motor state to the Pico."""
    global last_send_time, active_motors
    current_time = time.time()
    
    # We allow the send if it's a manual burst (after 20ms) OR 
    # if it's a heartbeat (after 100ms)
    
    motor_msg = message_pb2.motorCommand()
    (motor_msg.motor1, motor_msg.motor2, motor_msg.motor3, motor_msg.motor4,
     motor_msg.motor5, motor_msg.motor6, motor_msg.motor7, motor_msg.motor8) = values
    
    envelope = message_pb2.Envelope()
    envelope.motor_msg.CopyFrom(motor_msg)
    
    try:
        ser.write(cobs.encode(envelope.SerializeToString()) + b'\x00')
        active_motors = list(values)
        last_send_time = current_time
    except Exception:
        pass

def heartbeat_loop():
    """Independent thread to ensure a command is sent at least every 100ms."""
    while True:
        current_time = time.time()
        # If no command has been sent in the last 100ms, send the current active state
        if (current_time - last_send_time) >= HEARTBEAT_INTERVAL:
            # In staged mode, we send the last 'active' (committed) values.
            # In live/sine mode, we send the current 'motors' values.
            target = motors if (live_mode or sine_mode) else active_motors
            send_motors(target)
        time.sleep(0.01)

def listener():
    buffer = bytearray()
    while True:
        try:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                buffer.extend(data)
                while b'\x00' in buffer:
                    packet, buffer = buffer.split(b'\x00', 1)
                    if not packet: continue
                    try:
                        decoded = cobs.decode(packet)
                        env = message_pb2.Envelope()
                        env.ParseFromString(decoded)
                        ptype = env.WhichOneof("payload")
                        if ptype == "status":
                            pico_status["estop"] = env.status.estop
                            pico_status["manual"] = env.status.manual
                        elif ptype == "debug":
                            pico_status["debug"] = env.debug.content
                    except: pass
            time.sleep(0.01)
        except Exception: break

# --- 4. Main UI & Control ---
t_listen = threading.Thread(target=listener, daemon=True)
t_listen.start()

t_heart = threading.Thread(target=heartbeat_loop, daemon=True)
t_heart.start()

with term.fullscreen(), term.cbreak(), term.hidden_cursor():
    print(term.clear)
    while True:
        # Draw Header
        with term.location(0, 0):
            print(term.blue_bold + "--- Marine Robotics: Static Firmware Tester ---" + term.normal)
            print(f"Port: {detected_port} | {term.bold}R{term.normal}: Ramp | {term.bold}L{term.normal}: Live | {term.bold}T{term.normal}: Sine | {term.bold}Z{term.normal}: Zero")
            print("-" * term.width)

        # Draw Motors
        for i in range(8):
            with term.location(2, 4 + i):
                prefix = term.black_on_white + "> " + term.normal if i == selected_idx else "  "
                
                bar_len = int((motors[i] - 1000) / 1000 * 20)
                bar_len = max(0, min(20, bar_len))
                bar = "█" * bar_len + "░" * (20 - bar_len)
                
                label = f"Motor {i+1}"
                motor_label = term.cyan + label + term.normal if group_mode else label
                
                val_color = term.bold
                if motors[i] != active_motors[i]:
                    val_color = term.yellow_bold
                
                print(f"{prefix}{motor_label}: [{term.green}{bar}{term.normal}] {val_color}{motors[i]}{term.normal} us")

        # Draw Status Area
        with term.location(0, 14):
            print("-" * term.width)
            estop_color = term.red_bold if pico_status["estop"] else term.green
            mode_str = term.yellow_reverse + " LIVE " + term.normal if live_mode else "STAGED"
            sine_str = term.magenta_reverse + " SINE-ON " + term.normal if sine_mode else "OFF"
            
            print(f"MODE: {mode_str} | SINE: {sine_str} | GROUP: {term.cyan}{group_mode}{term.normal} | E-Stop: {estop_color}{pico_status['estop']}{term.normal}")
            print(f"LAST DEBUG: {term.yellow}{pico_status['debug']}{term.normal}")

        # Handle Input
        key = term.inkey(timeout=0.02)
        
        if key.lower() == 'q':
            break
        elif key.lower() == 's':
            sine_mode = False
            motors = [1500] * 8
            send_motors(motors)
        elif key.lower() == 'z':
            if group_mode:
                motors = [1500] * 8
            else:
                motors[selected_idx] = 1500
            if live_mode:
                send_motors(motors)
        elif key.lower() == 'g':
            group_mode = not group_mode
        elif key.lower() == 'l':
            live_mode = not live_mode
        elif key.lower() == 't':
            sine_mode = not sine_mode
        elif key.name == 'KEY_ENTER':
            send_motors(motors)
        
        # Ramp Logic
        elif key.lower() == 'r':
            while active_motors != motors:
                for i in range(8):
                    diff = motors[i] - active_motors[i]
                    if abs(diff) <= 50: active_motors[i] = motors[i]
                    else: active_motors[i] += 50 if diff > 0 else -50
                send_motors(active_motors)
                time.sleep(0.02)

        # Sinusoidal Test Logic
        if sine_mode:
            sine_offset += 0.2
            val = int(1500 + (math.sin(sine_offset) * 100))
            if group_mode:
                motors = [val] * 8
            else:
                motors[selected_idx] = val
            send_motors(motors)

        # Manual Adjustment Logic
        delta = 0
        if key.name == 'KEY_UP':    selected_idx = (selected_idx - 1) % 8
        elif key.name == 'KEY_DOWN':  selected_idx = (selected_idx + 1) % 8
        elif key.name == 'KEY_RIGHT': delta = 50
        elif key.name == 'KEY_LEFT':  delta = -50
        elif key.name == 'KEY_SRIGHT': delta = 100
        elif key.name == 'KEY_SLEFT':  delta = -100

        if delta != 0:
            if group_mode:
                for j in range(8):
                    motors[j] = max(1000, min(2000, motors[j] + delta))
            else:
                motors[selected_idx] = max(1000, min(2000, motors[selected_idx] + delta))
            
            if live_mode:
                send_motors(motors)

print("Testing Complete. Motors Neutralized.")