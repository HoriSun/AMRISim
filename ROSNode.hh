#ifndef AMRISIM_ROSNODE_HH
#define AMRISIM_ROSNODE_HH


/*  
 *  AMRISim is based on MobileSim (Copyright 2005 ActivMedia Robotics, 2006-2010 
 *  MobileRobots Inc, 2011-2015 Adept Technology, 2016-2017 Omron Adept Technologies)
 *  and Stage version 2 (Copyright Richard Vaughan, Brian Gerkey, Andrew Howard, and 
 *  others), published under the terms of the GNU General Public License version 2.
 *
 *  Copyright 2018 Reed Hedges and others
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include "AMRISim.hh"
#include "RobotInterface.hh"
#include "ros/ros.h"
#include "ros/callback_queue.h"
#include "ros/callback_queue_interface.h"
#include "geometry_msgs/Twist.h"
#include "tf/tf.h"
#include "tf/transform_broadcaster.h"
#include "tf/transform_datatypes.h"
#include "std_srvs/Empty.h"
#include "std_msgs/Bool.h"

/** @TODO XXX configurable frame IDs */

class ROSNode : public LogInterface {
private:
  RobotInterface *robot;

  ros::NodeHandle nodeHandle;
  ros::Publisher pose_pub;
  ros::Publisher bumpers_pub; 
  ros::Publisher sonar_pub;
  ros::Publisher sonar_pointcloud2_pub;
  bool publish_sonar, publish_sonar_pointcloud2;
  ros::Publisher motors_state_pub;
  std_msgs::Bool motors_state;
  bool published_motors_state;
  ros::Subscriber cmdvel_sub;
  ros::ServiceServer enable_srv;
  ros::ServiceServer disable_srv;
  bool enable_motors_cb(std_srvs::Empty::Request& request, std_srvs::Empty::Response& response);
  bool disable_motors_cb(std_srvs::Empty::Request& request, std_srvs::Empty::Response& response);

  //for odom->base_link transform
  tf::TransformBroadcaster odom_broadcaster;
  geometry_msgs::TransformStamped odom_trans;
  
  std::string frame_id_odom;
  std::string frame_id_base_link;
  std::string frame_id_bumper;
  std::string frame_id_sonar;

  class PublishCallback : public virtual ros::CallbackInterface {
    ROSNode *target;
  public:
    PublishCallback(ROSNode *rn) : target(rn) {}
    virtual ros::CallbackInterface::CallResult call() {
      target->publish();
      return CallResult::TryAgain;
    }
    virtual bool ready() {
      return true; // TODO could get hint from simulation loop in main about whether it's time, but this is not neccesary as long as that main loop calls ros::spinOnce() to determine interval.
    }
  };

 // PublishCallback publishCB;
  //boost::shared_ptr<PublishCallback> publishCBPtr;

  void cmdvel_cb(const geometry_msgs::TwistConstPtr &);

public:
  ROSNode(RobotInterface *r, AMRISim::Options* opts);
  ~ROSNode();
  bool start();
  void publish(); ///< Normally called automatically by ROS callback spin loop
};

#endif
