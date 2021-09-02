//
// Created by joaopedro on 30/08/21.
//

#include "dataset_manipulator.h"

//TODO: check possible save errors and retunr the output_string...
DatasetManipulator::DatasetManipulator() {

}

DatasetManipulator::~DatasetManipulator() {

}

bool DatasetManipulator::saveDataset(std::vector<Pose> _dataset,
                                     int _gripper_type,
                                     std::string _prefix,
                                     std::string _file_name_with_path,
                                     int _type){

    if(_type == EXPORT_EXTENSION::JSON)
        return exportJSONDataset(_dataset, _gripper_type, _prefix, _file_name_with_path);
    else
        return exportYAMLDataset(_dataset, _gripper_type, _prefix, _file_name_with_path);
}


bool DatasetManipulator::saveDataset(std::vector<Pose> _dataset,
                 std::string _prefix,
                 std::string _file_name_with_path,
                 int _type){

    if(_type == EXPORT_EXTENSION::JSON)
        return exportJSONDataset(_dataset, _prefix, _file_name_with_path);
    else
        return exportYAMLDataset(_dataset, _prefix, _file_name_with_path);
}



bool DatasetManipulator::exportJSONDataset(std::vector<Pose> _dataset, int _gripper_type, std::string _prefix, std::string _file_name_with_path) {

    std::string name_tag;
    json_pose_arr_.clear();

    for (size_t i = 0; i < _dataset.size() ; ++i) {

        name_tag = _prefix + std::to_string(i);
        json_pose_arr_[name_tag]["method"] = SYNTHESIS_METHOD::MIMIC_GRASPING;
        json_pose_arr_[name_tag]["gripper"] = _gripper_type;
        json_pose_arr_[name_tag]["parent_frame_id"] = _dataset.at(i).getParentName();
        json_pose_arr_[name_tag]["position"]["x"] = _dataset.at(i).getPosition().x();
        json_pose_arr_[name_tag]["position"]["y"] = _dataset.at(i).getPosition().y();
        json_pose_arr_[name_tag]["position"]["z"] = _dataset.at(i).getPosition().z();
        json_pose_arr_[name_tag]["orientation"]["x"] = _dataset.at(i).getQuaternionOrientation().x();
        json_pose_arr_[name_tag]["orientation"]["y"] = _dataset.at(i).getQuaternionOrientation().y();
        json_pose_arr_[name_tag]["orientation"]["z"] = _dataset.at(i).getQuaternionOrientation().z();
        json_pose_arr_[name_tag]["orientation"]["w"] = _dataset.at(i).getQuaternionOrientation().w();

    }
    std::ofstream outfile(_file_name_with_path);
    outfile << json_pose_arr_ << std::endl;
    outfile.close();
    return true;
}

bool DatasetManipulator::exportYAMLDataset(std::vector<Pose> _dataset, int _gripper_type, std::string _prefix, std::string _file_name_with_path) {

    std::ofstream file;
    std::stringstream toFile, dof_ss, parameters_ss;

    for (size_t i = 0; i < _dataset.size(); ++i) {

        dof_ss.str(""); //clear it
        dof_ss << "[]";

        // TODO: future work... There is no DOF in this situation
        /*
          for (size_t j = 0; j < g.candidates.at(i).gripper_data.dof.size(); ++j) {
            if(j == (g.candidates.at(i).gripper_data.dof.size()-1))
                dof_ss << g.candidates.at(i).gripper_data.dof.at(j) << "]" ;
            else
                dof_ss << g.candidates.at(i).gripper_data.dof.at(j) << ", " ;
        }
        */


        parameters_ss.str(""); //clear it
        parameters_ss << "[]";

        // TODO: future work... There is no Additional parameters in this situation since the pneumatic grippers only support ON-OFF
        /*
        for (size_t j = 0; j < g.candidates.at(i).gripper_data.parameters.size(); ++j) {
            if(j == (g.candidates.at(i).gripper_data.parameters.size()-1))
                parameters_ss << g.candidates.at(i).gripper_data.parameters.at(j) << "]" ;
            else
                parameters_ss << g.candidates.at(i).gripper_data.parameters.at(j) << ", " ;
        }
        */

        toFile << _prefix << std::to_string(i) << ":\n" <<
               "  method: " << "\n" <<
               "    type: " << SYNTHESIS_METHOD::MIMIC_GRASPING << "\n" <<
               "  gripper:" << "\n" <<
               "    type: " <<  _gripper_type << "\n" <<
               "    parameters: " << parameters_ss.str() << "\n" <<
               "    DOFs: " << dof_ss.str() << "\n" <<
               "  parent_frame_id: " << _dataset.at(i).getParentName() << "\n" <<
               "  position:" << "\n" <<
               "    x: " << _dataset.at(i).getPosition().x() << "\n" <<
               "    y: " << _dataset.at(i).getPosition().y() << "\n" <<
               "    z: " << _dataset.at(i).getPosition().z() << "\n" <<
               "  orientation:" << "\n" <<
               "    x: " << _dataset.at(i).getQuaternionOrientation().x() << "\n" <<
               "    y: " << _dataset.at(i).getQuaternionOrientation().y() << "\n" <<
               "    z: " << _dataset.at(i).getQuaternionOrientation().z() << "\n" <<
               "    w: " << _dataset.at(i).getQuaternionOrientation().w() << "\n";

    }

    file.open(_file_name_with_path);
    file << toFile.str();
    file.close();
    return true;

}

bool DatasetManipulator::exportJSONDataset(std::vector<Pose> _dataset, std::string _prefix, std::string _file_name_with_path) {

    std::string name_tag;
    json_pose_arr_.clear();

    for (size_t i = 0; i < _dataset.size() ; ++i) { // TODO: REMOVE: just to test

        name_tag = _prefix + std::to_string(i);
        json_pose_arr_[name_tag]["parent_frame_id"] = _dataset.at(i).getParentName();
        json_pose_arr_[name_tag]["position"]["x"] = _dataset.at(i).getPosition().x();
        json_pose_arr_[name_tag]["position"]["y"] = _dataset.at(i).getPosition().y();
        json_pose_arr_[name_tag]["position"]["z"] = _dataset.at(i).getPosition().z();
        json_pose_arr_[name_tag]["orientation"]["x"] = _dataset.at(i).getQuaternionOrientation().x();
        json_pose_arr_[name_tag]["orientation"]["y"] = _dataset.at(i).getQuaternionOrientation().y();
        json_pose_arr_[name_tag]["orientation"]["z"] = _dataset.at(i).getQuaternionOrientation().z();
        json_pose_arr_[name_tag]["orientation"]["w"] = _dataset.at(i).getQuaternionOrientation().w();

    }
    std::ofstream outfile(_file_name_with_path);
    outfile << json_pose_arr_ << std::endl;
    outfile.close();
    return true;
}

bool DatasetManipulator::exportYAMLDataset(std::vector<Pose> _dataset, std::string _prefix, std::string _file_name_with_path) {

    std::ofstream file;
    std::stringstream toFile, dof_ss, parameters_ss;

    for (size_t i = 0; i < _dataset.size(); ++i) {

        dof_ss.str(""); //clear it
        dof_ss << "[]";

        // TODO: future work... There is no DOF in this situation
        /*
          for (size_t j = 0; j < g.candidates.at(i).gripper_data.dof.size(); ++j) {
            if(j == (g.candidates.at(i).gripper_data.dof.size()-1))
                dof_ss << g.candidates.at(i).gripper_data.dof.at(j) << "]" ;
            else
                dof_ss << g.candidates.at(i).gripper_data.dof.at(j) << ", " ;
        }
        */


        parameters_ss.str(""); //clear it
        parameters_ss << "[]";

        // TODO: future work... There is no Additional parameters in this situation since the pneumatic grippers only support ON-OFF
        /*
        for (size_t j = 0; j < g.candidates.at(i).gripper_data.parameters.size(); ++j) {
            if(j == (g.candidates.at(i).gripper_data.parameters.size()-1))
                parameters_ss << g.candidates.at(i).gripper_data.parameters.at(j) << "]" ;
            else
                parameters_ss << g.candidates.at(i).gripper_data.parameters.at(j) << ", " ;
        }
        */

        toFile << _prefix << std::to_string(i) << ":\n" <<
               "  parent_frame_id: " << _dataset.at(i).getParentName() << "\n" <<
               "  position:" << "\n" <<
               "    x: " << _dataset.at(i).getPosition().x() << "\n" <<
               "    y: " << _dataset.at(i).getPosition().y() << "\n" <<
               "    z: " << _dataset.at(i).getPosition().z() << "\n" <<
               "  orientation:" << "\n" <<
               "    x: " << _dataset.at(i).getQuaternionOrientation().x() << "\n" <<
               "    y: " << _dataset.at(i).getQuaternionOrientation().y() << "\n" <<
               "    z: " << _dataset.at(i).getQuaternionOrientation().z() << "\n" <<
               "    w: " << _dataset.at(i).getQuaternionOrientation().w() << "\n";

    }

    file.open(_file_name_with_path);
    file << toFile.str();
    file.close();
    return true;

}