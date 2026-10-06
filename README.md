说明书简洁版
1. ## 系统数据流

整个系统由硬件层、驱动层、算法层和数据分析层构成，数据流如下：

**硬件层 (Livox MID360)**：
   雷达硬件通过网线连接电脑，持续扫描周围环境，产生原始的点云与IMU信号。

**驱动层 (livox_ros_driver2)**：
   驱动读取硬件信号，并转化为 ROS2 标准话题发布：
   - 发布点云数据到 `/livox/lidar` (自定义 CustomMsg 格式)
   - 发布惯性测量数据到 `/livox/imu`

**算法层 (FAST-LIO)**：
   FAST-LIO 订阅 `/livox/lidar` 和 `/livox/imu`，通过紧耦合的激光惯性里程计算法融合数据，推算出机器人的位置与姿态，并发布：
   - 机器人里程计数据 `/Odometry`
   - 坐标变换关系 `/tf` 和 `/tf_static`

**数据分析层 (bag_analysis 自定义节点)**：
   三个自定义节点订阅上述话题，进行二次分析与处理：
   - `odom_distance_node` 订阅 `/Odometry`，计算累计行驶距离。
   - `imu_shock_node` 订阅 `/livox/imu`，检测加速度突变/震动状态。
   - `time_latency_node` 订阅 `/livox/lidar`，计算数据采集与系统时间的延迟。
   这些节点将分析结果分别发布到 `/analysis/travel_distance`、`/analysis/imu_state` 和 `/analysis/time_latency`。

**可视化与记录 (RViz & Rosbag)**：
   - RViz 订阅点云、Odom和TF，进行实时可视化验证。
   - `rosbag` 录制所有关键Topic，用于后续的离线回放与节点调试。

2. ## 话题


本系统主要包含以下三类 Topic：

### 1. 传感器数据层（由 livox_ros_driver2 驱动发布）
| Topic 名称 | 消息类型 (Msg Type) | 作用说明 |
| :--- | :--- | :--- |
| `/livox/lidar` | `livox_ros_driver2/msg/CustomMsg` | 雷达扫描出的原始点云数据（10Hz） |
| `/livox/imu` | `sensor_msgs/msg/Imu` | 雷达内置 IMU 的加速度与角速度数据（约200Hz） |

### 2. 里程计与坐标变换层（由 FAST-LIO 算法发布）
| Topic 名称 | 消息类型 (Msg Type) | 作用说明 |
| :--- | :--- | :--- |
| `/Odometry` | `nav_msgs/msg/Odometry` | FAST-LIO 推算出的机器人位置、姿态与速度 |
| `/tf` | `tf2_msgs/msg/TFMessage` | 机器人各坐标系之间的动态变换关系 |
| `/tf_static` | `tf2_msgs/msg/TFMessage` | 机器人各组件之间固定的坐标变换关系 |

### 3. 数据分析层（由 bag_analysis 自定义节点发布）
| Topic 名称 | 消息类型 (Msg Type) | 作用说明 |
| :--- | :--- | :--- |
| `/analysis/travel_distance` | `std_msgs/msg/Float64` | 自定义节点计算的累计行驶距离（米） |
| `/analysis/imu_state` | `std_msgs/msg/String` | 自定义节点监测的 IMU 运动状态（SMOOTH/WARNING） |
| `/analysis/time_latency` | `std_msgs/msg/Float64` | 自定义节点计算的雷达数据延迟（毫秒） |

3. ## frame与TF关系
看图片吧
！[TF树](/home/sun/ros2_ws/src/bag_analysis/docs/TF_evidence/frames_2026-10-06_10.48.36.pdf)
![TF](/home/sun/ros2_ws/src/bag_analysis/docs/TF_evidence/Screenshot from 2026-10-06 10-49-37.png)

4. ## 节点功能

 `odom_distance_node`
   - **订阅**：`/Odometry`
   - **功能**：计算相邻帧位移并累加，得出总行驶距离。
   - **发布**：`/analysis/travel_distance` (Float64)

 `imu_shock_node`
   - **订阅**：`/livox/imu`
   - **功能**：检测相邻帧加速度突变，判断是否发生剧烈震动。
   - **发布**：`/analysis/imu_state` (String，输出 SMOOTH 或 WARNING)

 `time_latency_node`
   - **订阅**：`/livox/lidar`
   - **功能**：计算数据采集时间戳与系统当前时间的延迟（毫秒）。
   - **发布**：`/analysis/time_latency` (Float64)

5. ## 启动录制回放运行

>每次新开终端，务必先执行：`source ~/ros2_ws/install/setup.bash`

### 启动系统（实验室硬件环境）
- 启动雷达驱动：
  `ros2 launch livox_ros_driver2 msg_MID360_launch.py`
- 启动 FAST-LIO 算法：
  `ros2 launch fast_lio mapping.launch.py`
- 启动 RViz 可视化：
  `rviz2`

### 录制 rosbag
- 另开终端，录制全量数据：
  `ros2 bag record -o lab_bag /livox/lidar /livox/imu /Odometry /tf /tf_static`
- 录制结束后按 `Ctrl+C` 停止，随后可执行 `ros2 bag info lab_bag` 查看包信息。

### 离线回放与节点运行
- 终端1（播放数据）：
  `ros2 bag play lab_bag`
- 终端2（启动算法）：
  `ros2 launch fast_lio mapping.launch.py`
- 终端3（启动你的分析节点）：
  `ros2 launch bag_analysis analysis.launch.py`

  实际操作在视频中有体现

6. ## 如何验收

  `ros2 topic echo /analysis/travel_distance`
  `ros2 topic echo /analysis/imu_state`
  `ros2 topic echo /analysis/time_latency`

7. ## AI使用情况


本次实验在以下环节使用了 AI 辅助，所有最终代码均在本地编译验证并实机运行：

**环境配置与排错**：辅助排查 Ubuntu 下 Sophus、PCL 等库的编译依赖错误，修复官方驱动包（livox_ros_driver2）的 `package.xml` 和 `CMakeLists.txt` 配置冲突。
**代码框架生成**：辅助设计并生成三个 C++ 分析节点（Odom距离、IMU震动、时间戳延迟）的代码框架及 Launch 文件，由本人根据实际话题类型和参数进行修改、编译和测试。
**概念答疑与文档撰写**：辅助解释 LiDAR、IMU、Odom、TF 及 rosbag 等核心概念；辅助梳理并润色 README 中的系统数据流、Topic 列表及运行验证步骤。
**Git 操作指导**：辅助指导 Git 仓库初始化、`.gitignore` 配置及远程推送过程中的报错修复。
