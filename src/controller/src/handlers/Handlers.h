#ifndef HANDLERS_H
#define HANDLERS_H

//ROS messages
#include <sensor_msgs/Range.h>
#include <nav_msgs/Odometry.h>
#include <apriltags_ros/AprilTagDetectionArray.h>

//ROS libraries
#include <tf/transform_datatypes.h>
#include <angles/angles.h>

#include <vector>

#include "../Point.h"
#include "Tag.h"

//#include "../behaviors/Behaviors.h"
#include "../SMACS.h"
#include "../behaviors/AvoidBehavior.h"
#include "../behaviors/PickUpBehavior.h"
#include "../behaviors/DropBehavior.h"

#include <mutex>



/**
 * This executable file will contain all the handlers and their functionality
 */

/**
 * @brief The SonarHandler class - this singleton class will be responsible for storing and
 * handling the sonar events
 */
class SonarHandler{
    float minDistance; //if sonar is this distance from obstacle then put avoid behavior on stack
    bool isAvoidEnabled; //if obstacle avoid is off we will not do anything about obstacles. We will ignore
    static SonarHandler *s_instance; //static instance of class

    //values to hold sonar callbacks
    sensor_msgs::Range sonarLeft;
    sensor_msgs::Range sonarCenter;
    sensor_msgs::Range sonarRight;

    //private by default
    SonarHandler();

public:
    static SonarHandler* instance();
    void setEnable(const bool &isEnabled);
    bool const &isEnabled();

    //handlers
    void handleLeft(const sensor_msgs::Range::ConstPtr& sonarLeft);
    void handleCenter(const sensor_msgs::Range::ConstPtr& sonarCenter);
    void handleRight(const sensor_msgs::Range::ConstPtr& sonarRight);


    float getSonarLeft();
    float getSonarCenter();
    float getSonarRight();

    float getMinDistance();

};




class OdometryHandler{
    Point currentLocation;
    float linearVelocity;
    float angularVelocity;
    static OdometryHandler* s_instance;

    OdometryHandler();

public:
    static OdometryHandler* instance();

    void handle(const nav_msgs::Odometry::ConstPtr& message);
    float getTheta(){return currentLocation.theta;}
    float getX(){return currentLocation.x;}
    float getY(){return currentLocation.y;}
    float getLinear(){return linearVelocity;}
    float getAngular(){return angularVelocity;}

};


class EncoderHandler{
    int left;
    int right;
    static EncoderHandler* s_instance;

    EncoderHandler(){
        left = 0;
        right = 0;
    }

public:
    static EncoderHandler* instance();

    void handle(const geometry_msgs::Twist::ConstPtr& message);

    int getEncoderLeft(){return left;}
    int getEncoderRight(){return right;}

};




class TargetHandler{
    static TargetHandler* s_instance;
    std::vector<Tag> centerTagsList;
    std::vector<Tag> cubeTagsList;

    // This is needed to make handler thread safe
    std::mutex instanceMutex;

    TargetHandler();


    bool hasCube = false;
    bool isHandlerOn = true;

    float lastSeenBlockErrorYaw = 0;

public:
    static TargetHandler* instance();

    void handle(const apriltags_ros::AprilTagDetectionArray::ConstPtr& message);

    int getNumberOfCubeTags();
    int getNumberOfCenterTagsSeen();
    int getLastSeenBlockError();

    std::vector<Tag> getCubeTags();
    std::vector<Tag> getCenterTags();

    // set if camera handler is enabled
    void setEnabled(bool isHandlerOn){
        this->isHandlerOn = isHandlerOn;
    }

    void setHasCube(bool hasCube){
        this->hasCube=hasCube;
    }

    bool getHasCube(){
        return hasCube;
    }

};




















#endif
