// Copyright 2020 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <clock_publisher/clock_publisher.hpp>

#include <rclcpp/create_timer.hpp>

void ClockPublisher::onTimer()
{
  if (flag_) {
    const double dt_sec = 1.0 / static_cast<double>(rate_);
    clock_.clock = rclcpp::Time(clock_.clock) + rclcpp::Duration::from_seconds(dt_sec);
    // std::cerr << std::fixed << rclcpp::Time(clock_.clock).seconds() << std::endl;
    clock_pub_->publish(clock_);
  }
}

ClockPublisher::ClockPublisher(const rclcpp::NodeOptions & node_options)
: Node("clock_publisher", node_options)
{
  clock_pub_ = this->create_publisher<rosgraph_msgs::msg::Clock>("/clock", 1);
  rate_ = this->declare_parameter("rate", 200);

  engage_sub_ = create_subscription<autoware_vehicle_msgs::msg::Engage>(
    "/vehicle/engage", 1, std::bind(&ClockPublisher::onTrigger, this, std::placeholders::_1));

  // Timer
  auto timer_callback = std::bind(&ClockPublisher::onTimer, this);
  auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(1.0 / static_cast<double>(rate_)));

  timer_ = std::make_shared<rclcpp::GenericTimer<decltype(timer_callback)>>(
    this->get_clock(), period, std::move(timer_callback),
    this->get_node_base_interface()->get_context());
  this->get_node_timers_interface()->add_timer(timer_, nullptr);
}

#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(ClockPublisher)
