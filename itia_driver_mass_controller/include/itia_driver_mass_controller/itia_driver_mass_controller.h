
 // -------------------------------------------------------------------------------- 
 // Copyright (c) 2017 CNR-ITIA <iras@itia.cnr.it>
 // All rights reserved.
 //
 // Redistribution and use in source and binary forms, with or without
 // modification, are permitted provided that the following conditions are met:
 //
 // 1. Redistributions of source code must retain the above copyright notice,
 // this list of conditions and the following disclaimer.
 // 2. Redistributions in binary form must reproduce the above copyright
 // notice, this list of conditions and the following disclaimer in the
 // documentation and/or other materials provided with the distribution.
 // 3. Neither the name of mosquitto nor the names of its
 // contributors may be used to endorse or promote products derived from
 // this software without specific prior written permission.
 //
 //
 // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 // AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 // IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 // ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 // LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 // CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 // SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 // INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 // CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 // ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 // POSSIBILITY OF SUCH DAMAGE.
 // -------------------------------------------------------------------------------- 

#ifndef __itia_driver_mass_contoller__
#define __itia_driver_mass_contoller__

#include <controller_interface/controller.h>
#include <hardware_interface/joint_command_interface.h>
# include <itia_nodelet_hw_interface/nodelet_hw_interface.h>
#include <itia_controllers_and_filters/discrete_state_space.h>
# include <thread>
# include <mutex>
#include <boost/graph/graph_concepts.hpp>
# include <ros/ros.h>
# include <itia_rutils/itia_rutils.h>
# include <sensor_msgs/JointState.h>
#include <pluginlib/class_list_macros.h>

namespace itia
{
  namespace control
  {
    
    class DrivenMassController : public controller_interface::Controller<itia::control::PosVelEffJointInterface>
    {
    public:
      bool init(itia::control::PosVelEffJointInterface* hw, ros::NodeHandle& root_nh, ros::NodeHandle& controller_nh);
      void update(const ros::Time& time, const ros::Duration& period);
      void starting(const ros::Time& time);
      void stopping(const ros::Time& time);
      
    protected:
      ros::CallbackQueue m_queue;
      boost::shared_ptr<ros::AsyncSpinner> m_spinner;
      
      std::vector<hardware_interface::JointHandle> m_joint_handles;
      std::vector<itia::control::DiscreteStateSpacePtr> m_target_pos_filters;
      std::vector<itia::control::DiscreteStateSpacePtr> m_target_vel_filters;
      std::vector<itia::control::DiscreteStateSpacePtr> m_controllers;
      std::vector<itia::control::DiscreteStateSpacePtr> m_link_pos_filters;
      std::vector<itia::control::DiscreteStateSpacePtr> m_link_vel_filters;
      
      int m_nAx;
      
      std::vector<std::string> m_joint_names;
      
      
      std::vector<double> m_minimum_error;
      std::vector<double> m_target_motor_position;
      
      itia::control::PosVelEffJointInterface* m_hw;
      itia::rutils::MsgReceiver<sensor_msgs::JointState> m_fb_js_rec;
      itia::rutils::MsgReceiver<sensor_msgs::JointState> m_target_js_rec;
      itia::rutils::MsgReceiver<sensor_msgs::JointState> m_ffw_js_rec;
      
      ros::Subscriber  m_js_target_sub;
      ros::Subscriber  m_js_fb_sub;
      ros::Subscriber  m_js_ffw_sub;
      
      ros::Publisher m_link_publisher;
      bool m_publishing_link_topic;
      sensor_msgs::JointStatePtr m_link_msg;
      
      ros::NodeHandle m_root_nh;
      ros::NodeHandle m_controller_nh;
      bool m_configured;
      bool m_feedforward_active;
      
      void timerCallback(const ros::TimerEvent& event);
      void stopThreads();
      ~DrivenMassController();
    };
    
    
  }
}

# endif