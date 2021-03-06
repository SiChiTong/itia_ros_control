cmake_minimum_required(VERSION 2.8.3)
project(itia_virtual_sensor)
add_compile_options(-std=c++11 -funroll-loops -Wall -Ofast)
set(CMAKE_BUILD_TYPE Release)

find_package(catkin REQUIRED COMPONENTS
  controller_interface
  controller_manager
  hardware_interface
  itia_controllers_and_filters
  itia_dynamics_core
  itia_futils
  itia_nodelet_hw_interface
  itia_rutils
  pluginlib
  roscpp
  sensor_msgs
  diagnostic_msgs
)


catkin_package(
  INCLUDE_DIRS include
  LIBRARIES itia_single_joint_state_space_control
  CATKIN_DEPENDS  controller_interface
  controller_manager
  hardware_interface
  itia_controllers_and_filters
  itia_dynamics_core
  itia_futils
  itia_nodelet_hw_interface
  itia_rutils
  pluginlib
  roscpp
  sensor_msgs
  diagnostic_msgs
  DEPENDS 
)



include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
  src/${PROJECT_NAME}/itia_virtual_sensor.cpp
  src/${PROJECT_NAME}/itia_velocity_sensor.cpp
)
add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
)
