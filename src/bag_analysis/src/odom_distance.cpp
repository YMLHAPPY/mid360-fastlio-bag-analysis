#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "std_msgs/msg/float64.hpp"
#include <cmath>

class OdomDistance : public rclcpp::Node {
public:
    OdomDistance() : Node("odom_distance_node"), total_distance_(0.0), has_last_(false) {
        subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/Odometry", 10, std::bind(&OdomDistance::odom_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<std_msgs::msg::Float64>("/analysis/travel_distance", 10);
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        double current_x = msg->pose.pose.position.x;
        double current_y = msg->pose.pose.position.y;

        if (has_last_) {
            double dx = current_x - last_x_;
            double dy = current_y - last_y_;
            total_distance_ += std::sqrt(dx * dx + dy * dy);

            auto message = std_msgs::msg::Float64();
            message.data = total_distance_;
            publisher_->publish(message);
        }
        
        last_x_ = current_x;
        last_y_ = current_y;
        has_last_ = true;
    }

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
    double total_distance_;
    double last_x_;
    double last_y_;
    bool has_last_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OdomDistance>());
    rclcpp::shutdown();
    return 0;
}