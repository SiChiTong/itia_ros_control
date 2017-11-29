#include <itia_helios_controller/itia_helios_controller.h>

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(itia::motion::HeliosController, controller_interface::ControllerBase);

namespace itia
{
namespace motion
{
  
HeliosController::~HeliosController()
{

}

bool HeliosController::init(itia::control::PosVelEffJointInterface* hw, ros::NodeHandle& root_nh, ros::NodeHandle& controller_nh)
{
  m_root_nh = root_nh;
  m_controller_nh = controller_nh;
  m_hw = hw;
  
  
  std::string feedback_topic_name;
  
  if (!m_controller_nh.getParam("feedback_topic_name", feedback_topic_name))
  {
    ROS_FATAL_STREAM(m_controller_nh.getNamespace()+"/'feedback_topic_name' does not exist");
    ROS_FATAL("ERROR DURING STARTING CONTROLLER '%s'", m_controller_nh.getNamespace().c_str());
    return false;
  }
  
  m_js_fb_sub     = m_controller_nh.subscribe<sensor_msgs::JointState>(feedback_topic_name, 1, &itia::rutils::MsgReceiver<sensor_msgs::JointState>::callback, &m_fb_js_rec);
  
  if (!m_root_nh.getParam("/controlled_joint_names", m_joint_names))
  {
    ROS_FATAL_STREAM(m_controller_nh.getNamespace()+"/'joint_names' does not exist");
    ROS_FATAL("ERROR DURING STARTING CONTROLLER '%s'", m_controller_nh.getNamespace().c_str());
    return false;
  }
  
    
  m_nAx = m_joint_names.size();
  
  
  return true;
}

void HeliosController::starting(const ros::Time& time)
{
  m_configured = false;  
 
  Eigen::VectorXd qini(m_nAx);
  qini.setZero();
  m_motion.resize(3, m_nAx);
  m_motion.setZero();
  
  ROS_INFO("waiting");
  ros::WallTime t0 = ros::WallTime::now();
  while (!m_fb_js_rec.isANewDataAvailable() && (ros::WallTime::now() - t0).toSec() <10 )
    ros::WallDuration(0.01).sleep();
  if (!m_fb_js_rec.isANewDataAvailable())
    return;
//   if (!m_fb_js_rec.waitForANewData(10))
//   {
//     m_configured = false;
//     return;
//   } 
  
  if (m_fb_js_rec.getData().position.size() != m_nAx)
  {
    ROS_FATAL("error feedback size is wrong");
    return;
  }
  for (int idx = 0;idx<m_nAx;idx++)
  {
    ROS_INFO("idx=%d, m_nAx=%d", idx, m_nAx);
    qini(idx) = m_fb_js_rec.getData().position.at(idx);
    ROS_INFO("idx=%d, m_nAx=%d", idx, m_nAx);
  }
  ROS_INFO("started");
  try 
  {
    m_planner.reset(new itia::helios::MpcPlanner(m_controller_nh, "helios", qini));
  }
  catch (const std::exception& e)
  {
    ROS_FATAL("FAILED DURING HELIOS LOADING: %s", e.what());
  }
  
  m_configured = true;
  ROS_INFO("started");
}

void HeliosController::stopping(const ros::Time& time)
{
  ROS_INFO("[ %s%s%s ] Stopping controller", BOLDGREEN, m_controller_nh.getNamespace().c_str(), RESET);
  m_configured = false;
}

void HeliosController::update(const ros::Time& time, const ros::Duration& period)
{
  if (m_configured)
  {
    m_motion = m_planner->update();
    for (int iDim = 0;iDim<m_nAx;iDim++)
    {
      m_hw->m_joint_target_msgs->position.at(iDim) = m_motion(0, iDim);
      m_hw->m_joint_target_msgs->velocity.at(iDim) = m_motion(1, iDim);
      m_hw->m_joint_target_msgs->effort.at(iDim)   = 0;                      //motion(2, iDim);
    }
    m_hw->m_joint_target_msgs->header.stamp = ros::Time::now();
  }
  else
    ROS_FATAL_THROTTLE(1, "[%s%s%s%s] PLANNER NOT CONFIGURED", BOLDRED, m_controller_nh.getNamespace().c_str(), RESET, RED);

}


}
}