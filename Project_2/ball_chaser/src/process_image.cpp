#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include "geometry_msgs/Twist.h"
#include <std_msgs/Float64.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Moving the arm to the center");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool ball_found = false;
    int ball_position = -1; // -1: not found, 0: left, 1: middle, 2: right

    // Loop through each pixel in the image and check if its equal to the first one
    for (int i = 0; i < img.height * img.step; i+=3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
            ball_found = true;
            int column = (i % img.step) / 3;

            // Determine the position of the ball
            if (column < img.width / 3) {
                ball_position = 0; // Left
            } else if (column < 2 * img.width / 3) {
                ball_position = 1; // Middle
            } else {
                ball_position = 2; // Right
            }
            break;
        }
    }

        // Drive the robot based on the ball position
    if (ball_found) {
        if (ball_position == 0) {
            drive_robot(0.0, 0.5); // Turn left
        } else if (ball_position == 1) {
            drive_robot(0.5, 0.0); // Move forward
        } else if (ball_position == 2) {
            drive_robot(0.0, -0.5); // Turn right
        }
    } else {
        drive_robot(0.0, 0.0); // Stop
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}