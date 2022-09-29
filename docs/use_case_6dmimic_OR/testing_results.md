
PhoXiControl

roslaunch grasp_selection_tester.launch

rostopic pub /object_recognition/PhotoneoSkill/goal photoneo_skill_msgs/PhotoneoSkillActionGoal "header:
  seq: 0
  stamp:
    secs: 0
    nsecs: 0
  frame_id: ''
goal_id:
  stamp:
    secs: 0
    nsecs: 0
  id: ''
goal:
  start: true"


 rostopic pub /object_recognition/ObjectR
 ecognitionSkill/goal object_recognition_skill_msgs/ObjectRecognitionSkillActionGoal "header:
   seq: 0
   stamp:
     secs: 0
     nsecs: 0
   frame_id: ''
 goal_id:
   stamp:
     secs: 0
     nsecs: 0
   id: ''
 goal:
   operationMode: ''
   objectModel: 'linear_groundtruth'
   tfNameForSortingClusters: ''
   clusterIndex: 0
   pcaCustomXFlipAxis: {x: 0.0, y: 0.0, z: 0.0}
   filteredPointcloudSaveFrameId: ''"


   rostopic pub /GraspEstimationSkill/goal grasp_estimation_skill_msgs/GraspEstimationSkillActionGoal "header:
     seq: 0
     stamp:
       secs: 0
       nsecs: 0
     frame_id: ''
   goal_id:
     stamp:
       secs: 0
       nsecs: 0
     id: ''
   goal:
     operation_mode: 0
     detected_object_name: 'support_bracket'
     detected_object_tf_name: 'object'"



     sh build_result_tf.sh

     rosrun mimic_grasping_ros_interface_package generate_abb_code robot_base support_bracket/candidate_ 1

place the abb_targets.txt into abb and test it
