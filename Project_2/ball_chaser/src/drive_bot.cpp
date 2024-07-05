#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <std_msgs/Float64.h>
#include "geometry_msgs/Twist.h"

ros::Publisher motor_cmd_pub;


// TODO: Create a handle_drive_request callback function that executes whenever a drive_bot service is requested
// This function should publish the requested linear x and angular velocities to the robot wheel joints
// After publishing the requested velocities, a message feedback should be returned with the requested wheel velocities

bool handle_drive_request(ball_chaser::DriveToTarget::Request& req,
    ball_chaser::DriveToTarget::Response& res)
{
    ROS_INFO("DriveToTarget Request Received");

    geometry_msgs::Twist motor_command;
    
    motor_command.linear.x = req.linear_x;
    motor_command.angular.z = req.angular_z;

    // Publish angles to drive the robot
    motor_cmd_pub.publish(motor_command);

    // Wait 1 seconds for arm to settle
    // ros::Duration(1).sleep();

    // Return a response message
    res.msg_feedback = "Service Resounce ::: liner_x: " + std::to_string(req.linear_x) + " , angular_z: " + std::to_string(req.angular_z);
    ROS_INFO_STREAM(res.msg_feedback);

    return true;
}


// main funciton 
int main(int argc, char** argv)
{
    // Initialize the arm_mover node and create a handle to it
    ros::init(argc, argv, "drive_bot");
    ros::NodeHandle n;

    // Define publishers to publish geometry_msgs::Twist messages to /cmd_vel topic
    motor_cmd_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // Define a ball_chaser service with a handle_safe_move_request callback function
    ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);
    ROS_INFO("Ready to send joint commands");

    // Handle ROS communication events
    ros::spin();

    return 0;
}
