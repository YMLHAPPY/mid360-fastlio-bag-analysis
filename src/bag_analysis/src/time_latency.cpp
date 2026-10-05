#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "std_msgs/msg/float64.hpp"

class TimeLatency : public rclcpp::Node {
public:
    TimeLatency() : Node("time_latency_node") {
        subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "/livox/lidar", 10, std::bind(&TimeLatency::lidar_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<std_msgs::msg::Float64>("/analysis/time_latency", 10);
    }

private:
    void lidar_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
        rclcpp::Time data_time(msg->header.stamp);
        rclcpp::Time now = this->now();
        
        double latency_ms = (now - data_time).seconds() * 1000.0;

        auto message = std_msgs::msg::Float64();
        message.data = latency_ms;
        publisher_->publish(message);
    }

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TimeLatency>());
    rclcpp::shutdown();
    return 0;
}