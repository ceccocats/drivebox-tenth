#ifndef DINONAV_H
#define DINONAV_H

#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/Odometry.h"

#include <dynamic_reconfigure/server.h>
#include <dino_nav/DinonavConfig.h>

struct dinonav_t {

    float   speed;
    int     inflation;
    int     stop_cost;
    int     grid_dim;
    float   zoom;
    bool    enable;

    float steer_cost, dist_gain, inflation_cost;
};

const int EMPTY = 0;
const int WALL = 100;

struct view_t {

    float x,y;
    float l;
    float cell_l;
};

struct car_t {

    float length;
    float width;
};

void reconf(dino_nav::DinonavConfig &config, uint32_t level);
void laser_recv(const sensor_msgs::LaserScan::ConstPtr& msg);
void pose_recv(const geometry_msgs::PoseStamped::ConstPtr& msg);
void odom_recv(const nav_msgs::Odometry::ConstPtr& msg);

void init_view(view_t &view, int size);
void init_car(car_t &car, view_t &view, float zoom);

#endif //DINONAV_H
