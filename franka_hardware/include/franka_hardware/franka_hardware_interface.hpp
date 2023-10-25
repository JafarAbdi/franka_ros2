// Copyright (c) 2021 Franka Emika GmbH
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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <hardware_interface/visibility_control.h>
#include <franka_hardware/robot.hpp>

#include <hardware_interface/hardware_info.hpp>
#include <hardware_interface/system_interface.hpp>
#include <hardware_interface/types/hardware_interface_return_values.hpp>
#include <rclcpp/logger.hpp>
#include <rclcpp/macros.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include "franka_hardware/franka_executor.hpp"
#include "franka_hardware/franka_param_service_server.hpp"

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

namespace franka_hardware {
class FrankaHardwareInterface : public hardware_interface::SystemInterface {
 public:
  explicit FrankaHardwareInterface(std::unique_ptr<Robot> robot);
  FrankaHardwareInterface() = default;

  hardware_interface::return_type prepare_command_mode_switch(
      const std::vector<std::string>& start_interfaces,
      const std::vector<std::string>& stop_interfaces) override;
  hardware_interface::return_type perform_command_mode_switch(
      const std::vector<std::string>& start_interfaces,
      const std::vector<std::string>& stop_interfaces) override;
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;
  CallbackReturn on_activate(const rclcpp_lifecycle::State& previous_state) override;
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State& previous_state) override;
  hardware_interface::return_type read(const rclcpp::Time& time,
                                       const rclcpp::Duration& period) override;
  hardware_interface::return_type write(const rclcpp::Time& time,
                                        const rclcpp::Duration& period) override;
  CallbackReturn on_init(const hardware_interface::HardwareInfo& info) override;
  static const size_t kNumberOfJoints = 7;

 private:
  std::shared_ptr<Robot> robot_;
  std::shared_ptr<FrankaParamServiceServer> node_;
  std::shared_ptr<FrankaExecutor> executor_;
  std::array<double, kNumberOfJoints> effort_commands_{0, 0, 0, 0, 0, 0, 0};
  std::array<double, kNumberOfJoints> position_commands_{0, 0, 0, 0, 0, 0, 0};
  std::array<double, kNumberOfJoints> hw_positions_{0, 0, 0, 0, 0, 0, 0};
  std::array<double, kNumberOfJoints> hw_velocities_{0, 0, 0, 0, 0, 0, 0};
  std::array<double, kNumberOfJoints> hw_efforts_{0, 0, 0, 0, 0, 0, 0};
  franka::RobotState hw_franka_robot_state_;
  franka::RobotState* hw_franka_robot_state_addr_ = &hw_franka_robot_state_;
  Model* hw_franka_model_ptr_ = nullptr;

  std::array<double, 6> hw_ft_sensor_measurements_{0, 0, 0, 0, 0, 0};
  franka_hardware::ControlMode control_mode_ = franka_hardware::ControlMode::None;
  // resources switching aux vars
  std::vector<std::string> stop_modes_;
  std::vector<std::string> start_modes_;

  bool effort_interface_claimed_ = false;
  bool position_interface_claimed_ = false;
  bool initialized_ = false;
  static rclcpp::Logger getLogger();

  const std::string k_robot_name{"panda"};
  const std::string k_robot_state_interface_name{"robot_state"};
  const std::string k_robot_model_interface_name{"robot_model"};

  // fault control
  double reset_fault_cmd_;
  double reset_fault_async_success_;
  double in_fault_;
  bool fault_controller_running_ = false;  // if fault controller running
  bool stop_fault_controller_ = false;
  bool start_fault_controller_ = false;
};
}  // namespace franka_hardware
