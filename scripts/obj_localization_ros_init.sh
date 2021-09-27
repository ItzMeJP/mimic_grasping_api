#!/bin/bash
unbuffer roslaunch mimic_grasping_ros_interface_package run.launch namespace:=object_localization use_publisher:=true --screen -v |& tee ~/.ros/log/mimic_grasping.log &
