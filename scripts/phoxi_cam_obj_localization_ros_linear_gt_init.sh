#!/bin/bash
PhoXiControl &
sleep 5
roslaunch mimic_grasping_ros_interface_package run.launch\
 namespace:=object_recognition\
 use_photoneo:=true\
 robot_acquistion_pose_name:=p2\
 max_registration_outliers:=0.2\
 max_registration_outliers_reference_pointcloud:=0.85\
 use_transformation_aligner:=false&
#it is mandatory to put & in the last command, otherwise the finish function will not work
