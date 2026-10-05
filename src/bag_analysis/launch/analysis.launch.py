from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 启动节点1：里程计距离计算
        Node(
            package='bag_analysis',
            executable='odom_distance_node',
            name='odom_distance_node',
            output='screen'
        ),
        # 启动节点2：IMU震动检测
        Node(
            package='bag_analysis',
            executable='imu_shock_node',
            name='imu_shock_node',
            output='screen'
        ),
        # 启动节点3：时间戳延迟监控
        Node(
            package='bag_analysis',
            executable='time_latency_node',
            name='time_latency_node',
            output='screen'
        )
    ])