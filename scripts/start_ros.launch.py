# Not recommended to run manually, this should be called by the start_ros.sh script

from launch import LaunchDescription
import launch_ros.actions

def generate_launch_description():

    return LaunchDescription([
        launch_ros.actions.Node(
            package='servo_pkg',
            namespace='servo_pkg',
            executable='servo_node',
            name='servo_node',
            remappings=[('/ctrl_pkg/servo_msg', '/cmdvel_to_servo_pkg/servo_msg')]
        ),

        launch_ros.actions.Node(
            package='cmdvel_to_servo_pkg',
            namespace='cmdvel_to_servo_pkg',
            executable='cmdvel_to_servo_node',
            name='cmdvel_to_servo_node'
        ),

        launch_ros.actions.Node(
            package='imu_pkg',
            namespace='imu_pkg',
            executable='imu_node',
            name='imu_node'
        )
    ])