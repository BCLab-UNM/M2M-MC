#include <ros/ros.h>

// ROS libraries
#include <angles/angles.h>
#include <random_numbers/random_numbers.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

// ROS messages
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Joy.h>
#include <sensor_msgs/Range.h>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <apriltags_ros/AprilTagDetectionArray.h>
#include <std_msgs/Float32MultiArray.h>
#include "swarmie_msgs/Waypoint.h"

#include <ros/ros.h>
#include <signal.h>

using namespace std;


int currentMode = 0;//mode of the robot. Manual or auto
char host[128];
string hostname(host);
string publishedName;




// Mothods that handle
void sigintEventHandler(int signal);
void modeHandler(const std_msgs::UInt8::ConstPtr& message);
void joyCmdHandler(const sensor_msgs::Joy::ConstPtr& message);
void publishHeartBeatTimerEventHandler(const ros::TimerEvent& event);
void publishStatusTimerEventHandler(const ros::TimerEvent& event);
void tick(const ros::TimerEvent&); //main tick of the robot

//Methods that publish
void sendDriveCommand(double left, double right);


//Publishers
ros::Publisher nodeTest;
ros::Publisher driveControlPublish;
ros::Publisher heartbeatPublisher;
ros::Publisher status_publisher;

//Subscribers
ros::Subscriber modeSubscriber;
ros::Subscriber joySubscriber;

//Times for ticking the stack
ros::Timer stateMachineTimer;
ros::Timer publish_status_timer;
ros::Timer publish_heartbeat_timer;
const float behaviourLoopTimeStep = 0.1; // time between the behaviour loop calls
const float status_publish_interval = 1;
const float heartbeat_publish_interval = 2;


//global variables that we are constantly use
//They are used a lot so they are global
geometry_msgs::Twist velocity;

int main(int argc, char **argv) {

  
    gethostname(host, sizeof (host));
    string hostname(host);

    if (argc >= 2) {
    publishedName = argv[1];
    cout << "Welcome to the world of tomorrow " << publishedName
         << "!  Behaviour turnDirectionule started." << endl;
    } else {
    publishedName = hostname;
    cout << "No Name Selected. Default is: " << publishedName << endl;
    }

    // NoSignalHandler so we can catch SIGINT ourselves and shutdown the node
    ros::init(argc, argv, (publishedName + "_CONTROLLER"), ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;

    // Register the SIGINT event handler so the node can shutdown properly
    signal(SIGINT, sigintEventHandler);

    driveControlPublish = nh.advertise<geometry_msgs::Twist>((publishedName + "/driveControl"), 10);
    heartbeatPublisher = nh.advertise<std_msgs::String>((publishedName + "/controller/heartbeat"), 1, true);
    status_publisher = nh.advertise<std_msgs::String>((publishedName + "/status"), 1, true);
    nodeTest = nh.advertise<std_msgs::String>((publishedName + "/test"), 1, true);

    modeSubscriber = nh.subscribe((publishedName + "/mode"), 1, modeHandler);
    joySubscriber = nh.subscribe((publishedName + "/joystick"), 10, joyCmdHandler);

    //Timers to publish some stuff.
    stateMachineTimer = nh.createTimer(ros::Duration(behaviourLoopTimeStep), tick);
    publish_status_timer = nh.createTimer(ros::Duration(status_publish_interval), publishStatusTimerEventHandler);
    publish_heartbeat_timer = nh.createTimer(ros::Duration(heartbeat_publish_interval), publishHeartBeatTimerEventHandler);



    ros::spin();

    return EXIT_SUCCESS;
}

void tick(const ros::TimerEvent&) {
    if (currentMode == 2 || currentMode == 3) {
        std_msgs::String msg;
        msg.data = "Tick tick tick";
        nodeTest.publish(msg);
    } else {
        std_msgs::String msg;
        msg.data = "Tick manual";
        nodeTest.publish(msg);
    }
}


void publishStatusTimerEventHandler(const ros::TimerEvent&) {
  std_msgs::String msg;
  msg.data = "online";
  status_publisher.publish(msg);
}

void modeHandler(const std_msgs::UInt8::ConstPtr& message) {
  currentMode = message->data;
}

void joyCmdHandler(const sensor_msgs::Joy::ConstPtr& message) {
  const int max_motor_cmd = 255;
  if (currentMode == 0 || currentMode == 1) {
    float linear  = abs(message->axes[4]) >= 0.1 ? message->axes[4]*max_motor_cmd : 0.0;
    float angular = abs(message->axes[3]) >= 0.1 ? message->axes[3]*max_motor_cmd : 0.0;

    float left = linear - angular;
    float right = linear + angular;

    if(left > max_motor_cmd) {
      left = max_motor_cmd;
    }
    else if(left < -max_motor_cmd) {
      left = -max_motor_cmd;
    }

    if(right > max_motor_cmd) {
      right = max_motor_cmd;
    }
    else if(right < -max_motor_cmd) {
      right = -max_motor_cmd;
    }

    sendDriveCommand(left, right);
  }
}

void sendDriveCommand(double left, double right)
{
  velocity.linear.x = left,
      velocity.angular.z = right;

  // publish the drive commands
  driveControlPublish.publish(velocity);
}

void sigintEventHandler(int sig) {
  // All the default sigint handler does is call shutdown()
  ros::shutdown();
}

void publishHeartBeatTimerEventHandler(const ros::TimerEvent&) {
  std_msgs::String msg;
  msg.data = "";
  heartbeatPublisher.publish(msg);
}


































