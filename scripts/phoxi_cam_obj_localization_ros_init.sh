#!/bin/bash
PhoXiControl &

roslaunch mimic_grasping_ros_interface_package run.launch namespace:=object_localization use_photoneo:=true
