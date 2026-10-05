#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "std_msgs/msg/string.hpp"
#include <cmath>

class ImuShock : public rclcpp::Node {
public:
    ImuShock() : Node("imu_shock_node"), has_last_(false), threshold_(2.0) {
        subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/livox/imu", 10, std::bind(&ImuShock::imu_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<std_msgs::msg::String>("/analysis/imu_state", 10);
    }

private:
    void imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg) {
        double ax = msg->linear_acceleration.x;
        double ay = msg->linear_acceleration.y;
        double az = msg->linear_acceleration.z;

        if (has_last_) {
            double delta = std::sqrt(std::pow(ax - last_ax_, 2) + 
                                     std::pow(ay - last_ay_, 2) + 
                                     std::pow(az - last_az_, 2));

            auto message = std_msgs::msg::String();
            if (delta > threshold_) {
                message.data = "WARNING: Sudden motion/vibration detected! (Delta: " + std::to_string(delta) + ")";
            } else {
                message.data = "SMOOTH (Delta: " + std::to_string(delta) + ")";
            }
            publisher_->publish(message);
        }

        last_ax_ = ax;
        last_ay_ = ay;
        last_az_ = az;
        has_last_ = true;
    }

    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    double last_ax_;
    double last_ay_;
    double last_az_;
    double threshold_;
    bool has_last_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImuShock>());
    rclcpp::shutdown();
    return 0;
}