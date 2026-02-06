from pathlib import Path
import serial

from cobs import cobs
from cobs.cobs import DecodeError as CobsDecodeError

from google.protobuf.message import DecodeError as ProtobufDecodeError

from abc import ABC

class BaseBridge(ABC):
    """
    Abstract class for bridge between ROS and a microcontroller
    
    Gives generic functionality for reading and writing over serial,
    but each concrete class must implement the encoding / decoding for
    each protobuf message type you wish to use
    """

    def __init__(self,
                 port: Path = Path('/dev/ttyUSB0'),
                 baud: int = 115200,
                 rx_timeout_sec: float = 0.05,
                 tx_label = BOARD_JETSON_ID,
                 rx_label = BOARD_PRESTO_ID

    ) -> None:

        self._device = serial.Serial(str(port), baud, timeout=rx_timeout_sec)
        self._device_info = {}

        self._buffer: bytes | None = None
        self._connected: bool = self._device.is_open

    def _read(self) -> bytes | None:
        """Read data"""
        frame = self._device.read_until(b'\x00')

        if not frame:
            return None
        frame = frame[:-1]  # Remove 0x00

        try:
            data = cobs.decode(frame)
            return data
        except CobsDecodeError:
            return None

    def _send(self, data: bytes) -> None:
        """Write data"""
        encoded = cobs.encode(data)
        self._device.write(encoded + b'\x00')

    # Every project should have an Envelope class but it will differ slighly per project
    def _send_envelope(self, envelope: Envelope) -> None:
        """Serialize and send an Envelope message"""    
        self._send(envelope.SerializeToString())

    def _read_envelope_once(self) -> Envelope | None:
        """Try to read one Envelope from the wire; return msg or None"""

        data = self._read()
        if not data:
            return None
        
        env = Envelope()
        try:
            env.ParseFromString(data)
        except ProtobufDecodeError:
            return None

        if env.Header.id == self.rx_label:
            return env

        return None
    
    def close(self) -> None:
        try:
            self._device.close()
        except Exception:
            pass
    

# ------- Concreate MCU Bridges ------

from ROSBridgeStarter.pb.robosub_pb2 import (
    Header,
    MotorCommand,
    IndicatorLightCommand,
    PrestoState,
    SensorBState,
    Envelope,
    BOARD_JETSON_ID,
    BOARD_PRESTO_ID,
    BOARD_SENSORB_ID
)

class PrestoBridge(BaseBridge):
    
    def __init__(self, 
                port = Path('/dev/ttyUSB0'), 
                baud = 115200, 
                rx_timeout_sec = 0.05
    ) -> None:
        super().__init__(port, baud, rx_timeout_sec)

    # --------- Board Specific Encoding / Decoding ------
    def send_motor_command(self, motors8: list[float]) -> None:
        if len(motors8) != 8:
            raise ValueError("MotorCommand requires exactly 8 values")
        
        env = Envelope()
        env.header.src = BOARD_JETSON_ID

        msg: MotorCommand = env.motor_cmd
        (
            msg.motor_1, 
            msg.motor_2, 
            msg.motor_3, 
            msg.motor_4,
            msg.motor_5, 
            msg.motor_6, 
            msg.motor_7, 
            msg.motor_8
        ) = motors8

        self._send_envelope(env)

    def send_indicator_led_command(self, r: float, g: float, b: float) -> None:

        env = Envelope()
        env.header.src = BOARD_JETSON_ID

        msg: IndicatorLightCommand = env.indicator_cmd
        msg.r = r
        msg.g = g
        msg.b = b

        self._send_envelope(env)

    def read_state_once(self) -> PrestoState | None:
        """Try to read one PrestoState from the wire; return msg or None."""

        env = self._read_envelope_once()
        if not env:
            return None

        if env.WhichOneof("payload") == "presto_state":  # How we can detect what kind of data is coming in
            return env.presto_state
        return None

class SensorBoardBridge(BaseBridge):
    
    def __init__(self, 
                port = Path('/dev/ttyUSB1'), 
                baud = 115200, 
                rx_timeout_sec = 0.05, 
                tx_label=BOARD_JETSON_ID, 
                rx_label=BOARD_SENSORB_ID
    ) -> None:

       super().__init__(port, baud, rx_timeout_sec, tx_label, rx_label) 

    def read_state_once(self) -> SensorBState | None:
        """Read one SensorBState from the wire, if present."""
        env = self._read_envelope_once()
        if not env:
            return None

        if env.WhichOneof("payload") == "sensorb_state":
            return env.sensorb_state
        return None