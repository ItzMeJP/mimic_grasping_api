//
// Created by joaopedro on 30/08/21.
//

#ifndef MIMIC_GRASPING_API_DATASET_MANIPULATOR_H
#define MIMIC_GRASPING_API_DATASET_MANIPULATOR_H

#include <iostream>
#include <transform_manipulation/pose.h>
#include <transform_manipulation/transform.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>

class DatasetManipulator  {
public:
    DatasetManipulator();
    ~DatasetManipulator();

    enum EXPORT_EXTENSION{
        YAML,
        JSON,
    };

    enum GRIPPER_ID {        // Type of gripper used
        ROBOTIQ_2F_85,       // two adaptive fingers gripper from Robotiq with opening of 85mm
        ROBOTIQ_2F_140,      // two adaptive fingers gripper from Robotiq with opening of 140mm
        ROBOTIQ_3F,           // three adaptive fingers gripper from Robotiq
        SUCTION,
        FESTO_2F_HGPC_16_A,   //Pneumatic parallel 2F gripper from FESTO
        SCHMALZ_SINGLE_RECT_X_SUCTION,
    };

    enum SYNTHESIS_METHOD {
        MANUAL,
        SANN_MULTIFINGERED,
        SANN_SUCTION,
        MIMIC_GRASPING,
    };

    bool saveDataset(std::vector<Pose> _dataset,
                     std::string _prefix,
                     std::string _file_name_with_path,
                     int _type);

    bool saveDataset(std::vector<Pose> _dataset,
                     int _gripper_type,
                     std::string _prefix,
                     std::string _file_name_with_path,
                     int _type);

    std::string getDatasetManipulatorOutputSTR();
    bool loadTransformationMatrix(std::string _file_with_path);
    bool applyTransformation(std::vector<Pose> _obj_poses,
                             std::vector<Pose> _tool_poses,
                             std::vector<Pose> &_tool_poses_wrt_obj);

protected:
    Json::Value json_matrix_;
    Eigen::Matrix4d transformation_matrix_;


private:

    Json::Value json_pose_arr_;
    std::string output_string_;

    bool exportJSONDataset(std::vector<Pose> _dataset,
                           int gripper_type_,
                           std::string _prefix,
                           std::string _file_name_with_path);

    bool exportJSONDataset(std::vector<Pose> _dataset,
                           std::string _prefix,
                           std::string _file_name_with_path);

    bool exportYAMLDataset(std::vector<Pose> _dataset,
                           std::string _prefix,
                           std::string _file_name_with_path);

    bool exportYAMLDataset(std::vector<Pose> _dataset,
                           int gripper_type_,
                           std::string _prefix,
                           std::string _file_name_with_path);

};
#endif //MIMIC_GRASPING_API_DATASET_MANIPULATOR_H
