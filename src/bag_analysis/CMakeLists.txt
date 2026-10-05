cmake_minimum_required(VERSION 3.8)
project(bag_analysis)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 节点1：里程计距离
add_executable(odom_distance_node src/odom_distance.cpp)
ament_target_dependencies(odom_distance_node rclcpp nav_msgs std_msgs)
install(TARGETS odom_distance_node DESTINATION lib/${PROJECT_NAME})

# 节点2：IMU震动检测
add_executable(imu_shock_node src/imu_shock.cpp)
ament_target_dependencies(imu_shock_node rclcpp sensor_msgs std_msgs)
install(TARGETS imu_shock_node DESTINATION lib/${PROJECT_NAME})

# 节点3：时间戳延迟
add_executable(time_latency_node src/time_latency.cpp)
ament_target_dependencies(time_latency_node rclcpp sensor_msgs std_msgs)
install(TARGETS time_latency_node DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
        DESTINATION share/${PROJECT_NAME})

ament_package()
