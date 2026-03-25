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
motors = [0.0] * 8  
active_motors = [0.0] * 8  
selected_idx = 0
group_mode = False
live_mode = False
sine_mode = False
sine_offset = 0.0

# Stepping Logic
step_mode = "FINE"  # Can be "FINE" or "COARSE"
current_step = 0.05

last_send_time = 0
MIN_SEND_INTERVAL = 0.02  
HEARTBEAT_INTERVAL = 0.1   

pico_status = {"estop": False, "manual": False, "debug": "No messages yet"}

def clamp_and_round(val):
    return round(max(-1.0, min(1.0, val)), 2)

# --- 3. Communication Logic ---
def send_motors(values):
    global last_send_time, active_motors
    current_time = time.time()
    
    motor_msg = message_pb2.motorCommand()
    (motor_msg.motor1, motor_msg.motor2, motor_msg.motor3, motor_msg.motor4,
     motor_msg.motor5, motor_msg.motor6, motor_msg.motor7, motor_msg.motor8) = values
    
    envelope = message_pb2.Envelope()
    envelope.motor_msg.CopyFrom(motor_msg)
    
    try:
        ser.write(cobs.encode(envelope.SerializeToString()) + b'\x00')
        active_motors = [round(v, 2) for v in values]
        last_send_time = current_time
    except Exception:
        pass

def heartbeat_loop():
    while True:
        current_time = time.time()
        if (current_time - last_send_time) >= HEARTBEAT_INTERVAL:
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
        with term.location(0, 0):
            print(term.blue_bold + "--- Marine Robotics: Precise Float Tester ---" + term.normal)
            print(f"Port: {detected_port} | {term.bold}F{term.normal}: Toggle Step | {term.bold}T{term.normal}: Sine | {term.bold}Z{term.normal}: Zero | {term.bold}L{term.normal}: Live")
            print("-" * term.width)

        for i in range(8):
            with term.location(2, 4 + i):
                prefix = term.black_on_white + "> " + term.normal if i == selected_idx else "  "
                bar_width = 20
                center = bar_width // 2
                pos = int(motors[i] * center) + center
                pos = max(0, min(bar_width, pos))
                
                bar = list("░" * bar_width)
                bar[center] = "│"
                if pos > center:
                    for j in range(center + 1, pos + 1): bar[j-1] = "█"
                elif pos < center:
                    for j in range(pos, center): bar[j] = "█"
                
                bar_str = "".join(bar)
                label = f"Motor {i+1}"
                motor_label = term.cyan + label + term.normal if group_mode else label
                
                val_color = term.bold
                if abs(motors[i] - active_motors[i]) > 0.001:
                    val_color = term.yellow_bold
                
                print(f"{prefix}{motor_label}: [{term.green}{bar_str}{term.normal}] {val_color}{motors[i]:.2f}{term.normal}  ")

        with term.location(0, 14):
            print("-" * term.width)
            estop_color = term.red_bold if pico_status["estop"] else term.green
            mode_str = term.yellow_reverse + " LIVE " + term.normal if live_mode else "STAGED"
            sine_str = term.magenta_reverse + " SINE-ON " + term.normal if sine_mode else "OFF"
            step_display = term.white_on_blue + f" {step_mode} ({current_step}) " + term.normal
            
            print(f"MODE: {mode_str} | SINE: {sine_str} | STEP: {step_display} | GROUP: {term.cyan}{group_mode}{term.normal}")
            print(f"E-Stop: {estop_color}{pico_status['estop']}{term.normal} | LAST DEBUG: {term.yellow}{pico_status['debug']}{term.normal}")

        key = term.inkey(timeout=0.02)
        if key.lower() == 'q': break
        elif key.lower() == 's':
            sine_mode = False
            motors = [0.0] * 8
            send_motors(motors)
        elif key.lower() == 'f':
            if step_mode == "FINE":
                step_mode = "COARSE"
                current_step = 0.25
            else:
                step_mode = "FINE"
                current_step = 0.05
        elif key.lower() == 'z':
            if group_mode: motors = [0.0] * 8
            else: motors[selected_idx] = 0.0
            if live_mode: send_motors(motors)
        elif key.lower() == 'g': group_mode = not group_mode
        elif key.lower() == 'l': live_mode = not live_mode
        elif key.lower() == 't': sine_mode = not sine_mode
        elif key.name == 'KEY_ENTER': send_motors(motors)
        
        elif key.lower() == 'r':
            while active_motors != motors:
                for i in range(8):
                    diff = motors[i] - active_motors[i]
                    ramp_step = 0.05
                    if abs(diff) <= ramp_step: active_motors[i] = motors[i]
                    else: active_motors[i] = clamp_and_round(active_motors[i] + (ramp_step if diff > 0 else -ramp_step))
                send_motors(active_motors)
                time.sleep(0.02)

        # Sinusoidal Test Logic - Full Sweep (-1 to 1)
        if sine_mode:
            sine_offset += 0.1 # Slower increment for a smoother full sweep
            val = clamp_and_round(math.sin(sine_offset)) 
            if group_mode: motors = [val] * 8
            else: motors[selected_idx] = val
            send_motors(motors)

        delta = 0
        if key.name == 'KEY_UP':    selected_idx = (selected_idx - 1) % 8
        elif key.name == 'KEY_DOWN':  selected_idx = (selected_idx + 1) % 8
        elif key.name == 'KEY_RIGHT': delta = current_step
        elif key.name == 'KEY_LEFT':  delta = -current_step

        if delta != 0:
            if group_mode:
                for j in range(8): motors[j] = clamp_and_round(motors[j] + delta)
            else:
                motors[selected_idx] = clamp_and_round(motors[selected_idx] + delta)
            if live_mode: send_motors(motors)

print("Testing Complete. Motors Neutralized.")