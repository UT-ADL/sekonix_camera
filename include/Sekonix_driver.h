// Created by Maxandre Ogeret.
// (c) 2022 University of Tartu - Autonomous Driving Lab.

#ifndef SEKONIX_CAMERA_UT_SEKONIX_DRIVER_H
#define SEKONIX_CAMERA_UT_SEKONIX_DRIVER_H

#include <yaml-cpp/yaml.h>
#include <dw/sensors/Sensors.h>
#include <ros/package.h>
#include <ros/ros.h>

#include "cameras/CameraH264.h"
#include "cameras/CameraJpg.h"
#include <DriveworksApiWrapper.h>
#include "thread_pool.hpp"

#include "exceptions/SekonixDriverFatalException.h"
#include "exceptions/SekonixDriverMinorException.h"

class Sekonix_driver
{
public:
  /**
   * Constructor
   * @param nodehandle
   */
  Sekonix_driver(ros::NodeHandle* nodehandle);

  /**
   * @brief Initializes all parameters and config, handles exception.
   */
  void setup_cameras();

  /**
   * @brief Polls camera, publishes images and handles exception.
   */
  void poll_and_process();

private:
  static constexpr size_t MAX_TRIES_ = 100;
  bool all_cameras_valid_;
  std::unique_ptr<thread_pool> pool_;
  std::vector<std::future<bool>> future_pool_;
  size_t tries_ = 0;
  ros::NodeHandle nh_;
  std::string encoder_name_;
  std::string config_file_path_;
  YAML::Node config_;
  std::shared_ptr<DriveworksApiWrapper> driveworksApiWrapper_;
  size_t camera_count = 0;
  std::vector<std::shared_ptr<CameraBase>> camera_vector_;
};

#endif  // SEKONIX_CAMERA_UT_SEKONIX_DRIVER_H
