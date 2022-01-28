echo "Killing ROS Nodes"
#pgrep --exact "object_recognit"
#status=$?

#while [ $status -eq 1 ]; do
#  pgrep --exact "object_recognit"
#  status=$?
#  echo "Trying to kill Nodes. Aux: " $status
#  sleep 1
#  rosnode kill -a
#done
rosnode kill -a&
echo "Killed ROS nodes"
sleep 0.2
echo "Killing Rviz"
pkill rviz
sleep 0.2
echo "Killing Rosmaster"
pkill rosmaster
echo "Killing PhoxiControl"
pkill PhoXiControl
sleep 0.2
