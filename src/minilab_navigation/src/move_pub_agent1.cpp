#include <ros/ros.h>
#include <tf/tf.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/Odometry.h>
#include <math.h>
//#include <std_msgs/Float64.h>

geometry_msgs::Pose2D current_pose_leader;
ros::Time tk0_1;
float px_leader,py_leader,vx_leader;
void odomCallbackLeader(const nav_msgs::OdometryConstPtr& msg)
{
    // linear position
    current_pose_leader.x = msg->pose.pose.position.x;
    current_pose_leader.y = msg->pose.pose.position.y;
    tk0_1 = msg->header.stamp;
    px_leader=current_pose_leader.x;
    py_leader=current_pose_leader.y;
    vx_leader = msg->twist.twist.linear.x;
   
    // quaternion to RPY conversion
    tf::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    // angular position
    current_pose_leader.theta = yaw;
    
}
geometry_msgs::Pose2D current_pose_agent3;
ros::Time tk3_1;
float px_agent3,py_agent3,vx_agent3;
void odomCallbackagent3(const nav_msgs::OdometryConstPtr& msg)
{
    // linear position
    current_pose_agent3.x = msg->pose.pose.position.x;
    current_pose_agent3.y = msg->pose.pose.position.y;
    tk3_1 = msg->header.stamp;
    px_agent3=current_pose_agent3.x;
    py_agent3=current_pose_agent3.y;
    vx_agent3 = msg->twist.twist.linear.x;
   
    // quaternion to RPY conversion
    tf::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    // angular position
    current_pose_agent3.theta = yaw;
    
}
geometry_msgs::Pose2D current_pose_agent1;

ros::Time tk1_1;
float px_agent1,py_agent1,vx_agent1;
void odomCallbackagent1(const nav_msgs::OdometryConstPtr& msg)
{
    // linear position
    current_pose_agent1.x = msg->pose.pose.position.x;
    current_pose_agent1.y = msg->pose.pose.position.y;
    tk1_1 = msg->header.stamp;
    px_agent1=current_pose_agent1.x;
    py_agent1=current_pose_agent1.y;
    vx_agent1 = msg->twist.twist.linear.x;
    // quaternion to RPY conversion
    tf::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    // angular position
    current_pose_leader.theta = yaw;
    
}

int main(int argc, char **argv)
{
    const double PI = 3.14159265358979323846;
    double u,u_integral = 0.0;
    ros::Time int_time,current_time,prev_time;
    ros::Duration delta_t;
    double dvhat0_1,vhat0_1=0,dpxhat0_1,pxhat0_1=0,z0=0;
    double dvhat1_1,vhat1_1=0,dpxhat1_1,pxhat1_1=0,z1=0;
    double dvhat3_1,vhat3_1=0,dpxhat3_1,pxhat3_1=0,z3=0;
    double theta=3,lembda=.5;
   
    ROS_INFO("start");

    ros::init(argc, argv, "move_pub_agent1");
    ros::NodeHandle n;
    
    ros::Subscriber sub_odometryAgent1 = n.subscribe("/minilab1/odom", 10, odomCallbackagent1);
    //ros::Subscriber sub_odometryAgent3 = n.subscribe("/agent3/odom", 10, odomCallbackagent3);
    ros::Subscriber sub_odometry = n.subscribe("/minilab3/odom", 10, odomCallbackLeader);
    ros::Publisher movement_pubAgent1 = n.advertise<geometry_msgs::Twist>("/minilab1/cmd_vel",10); 

    //for sensors the value after , should be higher to get a more accurate result (queued)
    
    ros::Rate rate(10); //
    
    
    //ros::Publisher input_pubAgent1 = n.advertise<std_msgs::Float64>("/agent1/input",10); 

    //move Agent 1
    //ROS_INFO("stamp: [%f]", tk0_1_sec);
    ROS_INFO("move forward 1 meter");
    
    while(ros::ok())
    {
	geometry_msgs::Twist move;
	//std_msgs::Float64 input;
	ros::Time t = ros::Time::now();
   	// calculate delta_t
	delta_t = t - prev_time;
	prev_time = ros::Time::now();
        //Observer to estimate leader's states
        z0=exp(-2*theta*(t.toSec()-tk0_1.toSec()))*(pxhat0_1-px_leader);
        dvhat0_1=-theta*theta*z0;
	dpxhat0_1=vhat0_1-2*theta*z0;
	vhat0_1 +=dvhat0_1*delta_t.toSec(); // estimated velocity of the leader
	pxhat0_1 +=dpxhat0_1*delta_t.toSec();// estimated position of the leader
        //Observer to estimate agent1's states
        z1=exp(-2*theta*(t.toSec()-tk1_1.toSec()))*(pxhat1_1-px_agent1);
        dvhat1_1=-theta*theta*z1;
	dpxhat1_1=vhat1_1-2*theta*z1;
	vhat1_1 +=dvhat1_1*delta_t.toSec(); // estimated velocity of the agent1
	pxhat1_1 +=dpxhat1_1*delta_t.toSec();// estimated position of the agent1
        //Observer to estimate leader's states
        	//z3=exp(-2*theta*(t.toSec()-tk3_1.toSec()))*(pxhat3_1-px_agent3);
        	//dvhat3_1=-theta*theta*z3;
		//dpxhat3_1=vhat3_1-2*theta*z3;
		//vhat3_1 +=dvhat3_1*delta_t.toSec(); // estimated velocity of the leader
		//pxhat3_1 +=dpxhat3_1*delta_t.toSec();// estimated position of the leader
	
	// calculating u
	//u=-(px_agent1-px_leader)-(vx_agent1-vx_leader);
	u=-(lembda*lembda)*(pxhat1_1-pxhat0_1)-lembda*(vhat1_1-vhat0_1);
	u_integral += u*delta_t.toSec();//velocity
        //velocity controls
        move.linear.x = u_integral; //speed value m/s
        move.angular.z = 0;
        movement_pubAgent1.publish(move);
	//input.data=u;
	//input_pubAgent1.publish(input);
        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
