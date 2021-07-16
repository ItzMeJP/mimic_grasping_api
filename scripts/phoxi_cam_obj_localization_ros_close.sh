#!/bin/bash
echo "Killing PhoxiControl"
pkill PhoXiControl
sleep 0.2
echo "Killing Rviz"
pkill rviz
sleep 0.2
echo "Killing ROS Nodes"
rosnode kill -a
sleep 0.5
echo "Killing Rosout"
pkill rosout
sleep 0.2
echo "Killing Rosmaster"
pkill rosmaster
