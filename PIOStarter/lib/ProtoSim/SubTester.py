import serial
import serial.tools.list_ports
from cobs import cobs
import message_pb2
import threading
import time
import sys
import math
from blessed import Terminal

# --- 1. Serial Discovery ---
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

# Dropper: 0=Neutral, 1=Open, 2=Close
dropper_val = 0
dropper_map = {0: "NEUTRAL", 1: "OPEN", 2: "CLOSE"}

step_mode = "FINE"
current_step = 0.05

pico_status = {"estop": False, "manual": False, "debug": "No messages yet", "debug_id": 0}
sensors = {"temp": 0.0, "amp": 0.0, "accel": {"x": 0.0, "y": 0.0, "z": 0.0}}

last_send_time = 0
HEARTBEAT_INTERVAL = 0.1   

def clamp_and_round(val):
    return round(max(-1.0, min(1.0, val)), 2)

# --- 3. Communication Logic ---
def send_payload(payload_type, data):
    """Unified sender for Motor and Dropper commands."""
    global last_send_time, active_motors
    envelope = message_pb2.Envelope()
    
    if payload_type == "motor":
        msg = message_pb2.motorCommand()
        (msg.motor1, msg.motor2, msg.motor3, msg.motor4,
         msg.motor5, msg.motor6, msg.motor7, msg.motor8) = data
        envelope.motor_msg.CopyFrom(msg)
        active_motors = [round(v, 2) for v in data]
    elif payload_type == "dropper":
        msg = message_pb2.dropperCommand()
        msg.dropper = data
        envelope.dropper.CopyFrom(msg)
    
    try:
        ser.write(cobs.encode(envelope.SerializeToString()) + b'\x00')
        last_send_time = time.time()
    except Exception:
        pass

def heartbeat_loop():
    while True:
        if (time.time() - last_send_time) >= HEARTBEAT_INTERVAL:
            target = motors if (live_mode or sine_mode) else active_motors
            send_payload("motor", target)
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
                            pico_status["estop"], pico_status["manual"] = env.status.estop, env.status.manual
                        elif ptype == "debug":
                            pico_status["debug_id"], pico_status["debug"] = env.debug.id, env.debug.content
                        elif ptype == "readings":
                            stype = env.readings.WhichOneof("data")
                            if stype == "temperature": sensors["temp"] = env.readings.temperature
                            elif stype == "amperage": sensors["amp"] = env.readings.amperage
                            elif stype == "accel":
                                sensors["accel"] = {"x": env.readings.accel.x, "y": env.readings.accel.y, "z": env.readings.accel.z}
                    except: pass
            time.sleep(0.01)
        except Exception: break

# --- 4. UI Execution ---
threading.Thread(target=listener, daemon=True).start()
threading.Thread(target=heartbeat_loop, daemon=True).start()

with term.fullscreen(), term.cbreak(), term.hidden_cursor():
    print(term.clear)
    while True:
        with term.location(0, 0):
            print(term.blue_bold + "--- Marine Robotics: Static Firmware Tester ---" + term.normal)
            print(f"Port: {detected_port} | {term.bold}F{term.normal}: Step | {term.bold}D{term.normal}: Dropper | {term.bold}G{term.normal}: Group | {term.bold}ENTER{term.normal}: Send")
            print("-" * term.width)

        # Motor Bars (Left)
        for i in range(8):
            with term.location(2, 4 + i):
                prefix = term.black_on_white + "> " + term.normal if i == selected_idx else "  "
                bar_width = 20
                center = bar_width // 2
                pos = int(motors[i] * center) + center
                pos = max(0, min(bar_width, pos))
                bar = list("░" * bar_width); bar[center] = "│"
                if pos > center: 
                    for j in range(center + 1, pos + 1): bar[j-1] = "█"
                elif pos < center:
                    for j in range(pos, center): bar[j] = "█"
                
                motor_label = term.cyan + f"Motor {i+1}" + term.normal if group_mode else f"Motor {i+1}"
                val_color = term.yellow_bold if abs(motors[i] - active_motors[i]) > 0.001 else term.bold
                print(f"{prefix}{motor_label}: [{term.green}{''.join(bar)}{term.normal}] {val_color}{motors[i]:.2f}{term.normal}      ")

        # Telemetry (Right)
        with term.location(60, 4):
            print(term.bold + "SENSOR TELEMETRY" + term.normal)
            print(term.move_x(60) + f"Temp: {term.yellow}{sensors['temp']:.1f}°C{term.normal}   ")
            print(term.move_x(60) + f"Amp:  {term.red}{sensors['amp']:.2f} A{term.normal}    ")
            print(term.move_x(60) + f"Accel X: {sensors['accel']['x']:.2f} g  ")
            print(term.move_x(60) + f"Accel Y: {sensors['accel']['y']:.2f} g  ")
            print(term.move_x(60) + f"Accel Z: {sensors['accel']['z']:.2f} g  ")

        # Footer
        with term.location(0, 14):
            print(term.clear_eol + "-" * term.width)
            estop_col = term.red_bold if pico_status["estop"] else term.green
            mode_str = term.yellow_reverse + " LIVE " + term.normal if live_mode else "STAGED"
            drop_col = term.magenta_bold if dropper_val != 0 else term.normal
            print(term.clear_eol + f"MODE: {mode_str} | STEP: {step_mode} | DROPPER: {drop_col}{dropper_map[dropper_val]}{term.normal} | GROUP: {group_mode}")
            debug_info = f"[{pico_status['debug_id']}] {pico_status['debug']}"
            print(term.clear_eol + f"E-Stop: {estop_col}{pico_status['estop']}{term.normal} | LAST DEBUG: {term.yellow}{debug_info}{term.normal}")

        key = term.inkey(timeout=0.02)
        if key.lower() == 'q': break
        elif key.lower() == 'f': step_mode, current_step = ("COARSE", 0.25) if step_mode == "FINE" else ("FINE", 0.05)
        elif key.lower() == 'g': group_mode = not group_mode
        elif key.lower() == 'l': live_mode = not live_mode
        elif key.lower() == 't': sine_mode = not sine_mode
        elif key.lower() == 'z':
            motors = [0.0] * 8
            if live_mode: send_payload("motor", motors)
        elif key.lower() == 'd':
            dropper_val = (dropper_val + 1) % 3
            send_payload("dropper", dropper_val)
        elif key.name == 'KEY_ENTER': send_payload("motor", motors)

        # Controls
        delta = 0
        if key.name == 'KEY_UP': selected_idx = (selected_idx - 1) % 8
        elif key.name == 'KEY_DOWN': selected_idx = (selected_idx + 1) % 8
        elif key.name == 'KEY_RIGHT': delta = current_step
        elif key.name == 'KEY_LEFT': delta = -current_step

        if delta != 0:
            if group_mode: motors = [clamp_and_round(m + delta) for m in motors]
            else: motors[selected_idx] = clamp_and_round(motors[selected_idx] + delta)
            if live_mode: send_payload("motor", motors)

        if sine_mode:
            sine_offset += 0.1
            val = clamp_and_round(math.sin(sine_offset))
            if group_mode: motors = [val] * 8
            else: motors[selected_idx] = val
            send_payload("motor", motors)

print("Testing Complete.")