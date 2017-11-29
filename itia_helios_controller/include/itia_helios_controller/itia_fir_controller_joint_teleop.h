#ifndef __itia_helios_controller__
#define __itia_helios_controller__

#include <controller_interface/controller.h>
#include <hardware_interface/joint_command_interface.h>
# include <itia_nodelet_hw_interface/nodelet_hw_interface.h>
# include <thread>
# include <mutex>
#include <boost/graph/graph_concepts.hpp>
# include <ros/ros.h>
# include <itia_rutils/itia_rutils.h>
# include <sensor_msgs/JointState.h>
#include <pluginlib/class_list_macros.h>
# include <itia_helios/mpc_planner.h>
# include <itia_fir_planner/itia_fir_planner.h>
# include <std_msgs/Int64.h>
namespace itia
{
namespace motion
{
class JointTeleopFirController : public controller_interface::Controller<itia::control::PosVelEffJointInterface>
{
public:
  bool init(itia::control::PosVelEffJointInterface* hw, ros::NodeHandle& root_nh, ros::NodeHandle& controller_nh);
  void update(const ros::Time& time, const ros::Duration& period);
  void starting(const ros::Time& time);
  void stopping(const ros::Time& time);

  void overrideCallback(const std_msgs::Int64ConstPtr& msg);
  void safeOverrideCallback_1(const std_msgs::Int64ConstPtr& msg);
  void safeOverrideCallback_2(const std_msgs::Int64ConstPtr& msg);
  
  
protected:

  std::vector<hardware_interface::JointHandle> m_joint_handles;
  
  int m_nAx;
  
  std::vector<std::string> m_joint_names;
  itia::control::PosVelEffJointInterface* m_hw;
  itia::rutils::MsgReceiver<sensor_msgs::JointState> m_fb_js_rec;
  ros::Subscriber  m_js_fb_sub;
  ros::Publisher m_diagnostics_pub;
  
  itia::rutils::MsgReceiver<sensor_msgs::JointState> m_target_js_rec;
  ros::Subscriber  m_js_target_sub;
  
  ros::Duration m_message_timeout;
  ros::NodeHandle m_root_nh;
  ros::NodeHandle m_controller_nh;
  bool m_configured;
  itia::JMotion m_motion;
  
  ros::Subscriber m_override_topic;
  ros::Subscriber m_safe_override_topic_1;
  ros::Subscriber m_safe_override_topic_2;
  double m_override;
  double m_safe_override_1;
  double m_safe_override_2;
  
  Eigen::VectorXd m_upper_limit;
  Eigen::VectorXd m_lower_limit;
  
	itia::helios::FirPlannerPtr m_planner;
  
  ~JointTeleopFirController();
};


}
}

# endif