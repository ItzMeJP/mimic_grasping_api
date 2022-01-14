#!/bin/bash
PhoXiControl &
sleep 5
roslaunch mimic_grasping_ros_interface_package run.launch namespace:=object_recognition use_photoneo:=true
