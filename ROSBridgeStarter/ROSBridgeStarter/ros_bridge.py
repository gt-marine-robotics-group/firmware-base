from std_msgs.msg import Bool, ColorRGBA, Float32, Float32MultiArray
from rclpy.node import Node
import rclpy
from pathlib import Path
from ROSBridgeStarter.serial_base import PrestoBridge
from typing import Optional

import typer

ROS_IMPORT_SUCCESS = True
ROS_IMPORT_ERROR = None


class RosBridge(Node):
    def __init__(self,
                 presto_port: str = "/dev/ttyUSB0",
                 sesnorb_port: str = "/dev/ttyUSB1",
                 telemtry_hz: float = 20.0
                 ) -> None:

        super().__init__('firmware_bridge')

        self.presto_bridge: Optional[PrestoBridge] = None
        self.sensorb_bridge: Optional[PrestoBridge] = None

        try:
            self.presto_bridge = PrestoBridge(Path(presto_port))
        except Exception as e:
            self.get_logger().warn(
                f"Could not open presto port {presto_port}: {e}")

        try:
            self.sensorb_bridge = PrestoBridge(Path(sesnorb_port))
        except Exception as e:
            self.get_logger().warn(
                f"Could not open sensorboard port {sesnorb_port}: {e}")

        # ------ ROS Subscribers -------
        self.cmd_motors_sub = self.create_subscription(
            Float32MultiArray,
            "/pontus/thruster_cmds",
            self._motors_callback,
            10
        )

        self.cmd_indicator_led_sub = self.create_subscription(
            ColorRGBA,
            "/pontus/indicator_led_color",
            self._indicator_led_callback,
            10
        )

        self._last_led_color = None

        # ------ ROS Publishers ------
        self.estop_pub = self.create_publisher(
            Bool,
            "/pontus/e_stop",
            10
        )

        self.autonomy_switch_pub = self.create_publisher(
            Bool,
            "/pontus/autonomy_switch",
            10
        )

        # ------ Timers ------
        self.telemetry_timer = self.create_timer(
            1.0 / telemtry_hz,
            self._telemetry_timer_callback
        )

    # --------- Callbacks ---------

    def _motors_callback(self, msg: Float32MultiArray) -> None:
        vals = list(msg.data)

        if self.presto_bridge and self.presto_bridge._connected:
            try:
                self.presto_bridge.send_motor_command(vals)
            except Exception as e:
                self.get_logger().warn(f"send_motor_command failed: {e}")

    def _indicator_led_callback(self, msg: ColorRGBA) -> None:
        if self._last_led_color == msg:
            # Don't send color if the same as previous message
            return

        self._last_led_color = msg

        if self.presto_bridge and self.presto_bridge._connected:
            try:
                self.presto_bridge.send_indicator_led_command(
                    msg.r, msg.g, msg.b
                )
            except Exception as e:
                self.get_logger().warn(
                    f"send_indicator_led_command failed: {e}"
                )

    def _telemetry_timer_callback(self) -> None:
        # ------ Presto Board Telemetry ------
        if self.presto_bridge and self.presto_bridge._connected:
            state = self.presto_bridge.read_state_once()
            if state:
                e_stop_msg = Bool()
                e_stop_msg.data = bool(state.e_stop)
                self.estop_pub.publish(e_stop_msg)

                autonomy_switch_msg = Bool()
                autonomy_switch_msg.data = bool(state.autonomy_switch)
                self.autonomy_switch_pub.publish(autonomy_switch_msg)

        # ------ Sensor Board Telemetry ------
        if self.sensorb_driver and self.sensorb_driver._connected:
            state = self.sensorb_driver.read_state_once()
            if state:
                depth_msg = Float32()
                depth_msg.data = state.pressure_pa
                self.depth_sensor_pub.publish(depth_msg)

                voltage_msg = Float32()
                voltage_msg.data = state.voltage_v
                self.voltage_pub.publish(voltage_msg)

                current_msg = Float32()
                current_msg.data = state.current_a
                self.current_pub.publish(current_msg)

# ------ Main ------
def main(args: Optional[list[str]] = None) -> None:
    rclpy.init()

    node = RosBridge()

    try:
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()
