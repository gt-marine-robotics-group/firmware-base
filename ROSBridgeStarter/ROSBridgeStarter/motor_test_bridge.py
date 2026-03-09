from rclpy.node import Node
import rclpy

from std_msgs.msg import Float32MultiArray


class BridgeMotorState(Node):
    def __init__(self):

        super().__init__("ROS_bridge_motor_test")

        self.motor_state: float = 0    # Sweep across -1 to 1
        self.motor_dir: int = 1

        self.motor_pub = self.create_publisher(
            Float32MultiArray,
            "/thrust_cmds",
            10
        )

        self.sweep_timer = self.create_timer(
            1.0 / 20.0,
            self._sweep_timer_callback
        )

    def _sweep_timer_callback(self):
        self.motor_state += 0.1 * self.motor_dir

        if self.motor_state >= 1.0:
            self.motor_state = 1.0
            self.motor_dir = -1
        elif self.motor_state <= -1.0:
            self.motor_state = -1.0
            self.motor_dir = 1
        
        motor_cmds = Float32MultiArray()
        motor_cmds.data =  [
            self.motor_state,
            self.motor_state,
            self.motor_state,
            self.motor_state,
            self.motor_state,
            self.motor_state,
            self.motor_state,
            self.motor_state
        ]

        self.motor_pub.publish(motor_cmds)

def main(args = None) -> None:
    rclpy.init()

    node = BridgeMotorState()

    try:
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()
