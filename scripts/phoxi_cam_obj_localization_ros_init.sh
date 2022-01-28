#!/bin/bash
PhoXiControl &
sleep 5
roslaunch mimic_grasping_ros_interface_package run.launch namespace:=object_recognition use_photoneo:=true&
#it is mandatory to put & in the last command, otherwise the finish function will not work
