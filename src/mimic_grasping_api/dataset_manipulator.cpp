//
// Created by joaopedro on 30/08/21.
//

#include "dataset_manipulator.h"

namespace mimic_grasping {
    DatasetManipulator::DatasetManipulator() {

    }

    DatasetManipulator::~DatasetManipulator() {

    }

    std::string DatasetManipulator::getDatasetManipulatorOutputSTR() {
        return output_string_;
    }

    bool DatasetManipulator::loadTransformationMatrix(std::string _file_with_path) {

        std::ifstream config_file(_file_with_path, std::ifstream::binary);
        if (config_file) {
            try {
                config_file >> json_matrix_;
            } catch (const std::exception &e) {
                output_string_ = e.what();
                return false;
            }
        } else {
            output_string_ = "Matrix file not found. Current path: \" + _file";
            return false;
        }

        transformation_matrix_
                << json_matrix_["matrix"][0][0].asDouble(), json_matrix_["matrix"][0][1].asDouble(), json_matrix_["matrix"][0][2].asDouble(), json_matrix_["matrix"][0][3].asDouble(),
                json_matrix_["matrix"][1][0].asDouble(), json_matrix_["matrix"][1][1].asDouble(), json_matrix_["matrix"][1][2].asDouble(), json_matrix_["matrix"][1][3].asDouble(),
                json_matrix_["matrix"][2][0].asDouble(), json_matrix_["matrix"][2][1].asDouble(), json_matrix_["matrix"][2][2].asDouble(), json_matrix_["matrix"][2][3].asDouble(),
                json_matrix_["matrix"][3][0].asDouble(), json_matrix_["matrix"][3][1].asDouble(), json_matrix_["matrix"][3][2].asDouble(), json_matrix_["matrix"][3][3].asDouble();

        DEBUG_MSG("Loaded transformation matrix: \n" << transformation_matrix_ << "\n");

        return true;
    }

    bool DatasetManipulator::loadCompensationFileForOutputDataset(std::string _file_with_path){

        output_error_compensation_path_ = _file_with_path;

        if(!loadCompensationFile(_file_with_path)){
            output_string_ = "Error while loading compensation configuration file for output dataset.";
            DEBUG_MSG(output_string_);
            return false;
        }

        return true;
    }

    bool DatasetManipulator::saveTransformationMatrix(std::string _file_with_path){

        Json::Value jm;

        jm["matrix"][0][0] = 1; jm["matrix"][0][1] = 0; jm["matrix"][0][2] = 0; jm["matrix"][0][3] = 0;
        jm["matrix"][0][0] = 0; jm["matrix"][0][1] = 1; jm["matrix"][0][2] = 0; jm["matrix"][0][3] = 0;
        jm["matrix"][0][0] = 0; jm["matrix"][0][1] = 0; jm["matrix"][0][2] = 1; jm["matrix"][0][3] = 0;
        jm["matrix"][0][0] = 0; jm["matrix"][0][1] = 0; jm["matrix"][0][2] = 0; jm["matrix"][0][3] = 1;

        std::ofstream outfile(_file_with_path);
        outfile << jm << std::endl;
        outfile.close();

        return true;
    }

    bool DatasetManipulator::applyTransformation(Pose _obj_pose,
                             Pose _tool_pose,
                             Pose &_tool_pose_wrt_obj){

        Pose aux_pose;

        Transform T_obj_ref_to_tool_ref(_obj_pose.getParentName(), _tool_pose.getParentName(), transformation_matrix_);

        if (!T_obj_ref_to_tool_ref.apply(_obj_pose, aux_pose)) {
            output_string_ = "Error in transformation.";
            return false;
        }

        Transform obj_pose_wrt_tool_ref(aux_pose);

        if (!obj_pose_wrt_tool_ref.apply_inverse(_tool_pose, _tool_pose_wrt_obj)) {
            output_string_ = "Error in transformation.";
            return false;
        }

        DEBUG_MSG("Path to general output compensation file: " << output_error_compensation_path_);

        applyRunTimeLoadCorrection(output_error_compensation_path_,_tool_pose_wrt_obj);

        return true;

    }

    bool DatasetManipulator::applyTransformation(std::vector<Pose> _obj_poses,
                                                 std::vector<Pose> _tool_poses,
                                                 std::vector<Pose> &_tool_poses_wrt_obj_ref) {

        std::vector<Transform> obj_poses_wrt_tool_ref;
        Pose aux_pose;

        _tool_poses_wrt_obj_ref.clear();
        obj_poses_wrt_tool_ref.clear();

        if (_obj_poses.empty()) {
            output_string_ = "Object poses dataset is empty.";
            return false;
        }

        if (_tool_poses.empty()) {
            output_string_ = "Tool poses dataset is empty.";
            return false;
        }

        Transform T_obj_ref_to_tool_ref(_obj_poses.at(0).getParentName(), _tool_poses.at(0).getParentName(),
                                        transformation_matrix_);

        for (size_t i = 0; i < _obj_poses.size(); ++i) {

            if (!T_obj_ref_to_tool_ref.apply(_obj_poses.at(i), aux_pose)) {
                output_string_ = "Error in transformation.";
                return false;
            }

            Transform aux_transform(aux_pose);
            obj_poses_wrt_tool_ref.push_back(aux_transform);
        }

        if (_obj_poses.size() != 1 && _obj_poses.size() != _tool_poses.size()) {
            output_string_ = "Tool and Object dataset have different sizes for the chosen shoot_estimation method.";
            return false;
        }

        for (size_t i = 0; i < _tool_poses.size(); ++i) {

            Transform t = obj_poses_wrt_tool_ref.size() == 1 ? obj_poses_wrt_tool_ref.at(0) : obj_poses_wrt_tool_ref.at(
                    i); //TODO: the datasets should have the same size if the one_shoot_estimation is unset
            if (!t.apply_inverse(_tool_poses.at(i), aux_pose)) {
                output_string_ = "Error in transformation.";
                return false;
            }
            _tool_poses_wrt_obj_ref.push_back(aux_pose);

        }

        return true;
    }


    bool DatasetManipulator::saveDataset(std::vector<Pose> _dataset,
                                         int _gripper_type,
                                         std::string _prefix,
                                         std::string _file_name_with_path,
                                         int _type) {

        if (_type == EXPORT_EXTENSION::JSON)
            return exportJSONDataset(_dataset, _gripper_type, _prefix, _file_name_with_path);
        else
            return exportYAMLDataset(_dataset, _gripper_type, _prefix, _file_name_with_path);
    }


    bool DatasetManipulator::saveDataset(std::vector<Pose> _dataset,
                                         std::string _prefix,
                                         std::string _file_name_with_path,
                                         int _type) {

        if (_type == EXPORT_EXTENSION::JSON)
            return exportJSONDataset(_dataset, _prefix, _file_name_with_path);
        else
            return exportYAMLDataset(_dataset, _prefix, _file_name_with_path);
    }


    bool DatasetManipulator::exportJSONDataset(std::vector<Pose> _dataset, int _gripper_type, std::string _prefix,
                                               std::string _file_name_with_path) {

        std::string name_tag;
        json_pose_arr_.clear();

        if (_dataset.empty()) {
            output_string_ = "Cannot export an empty dataset.";
            return false;
        }

        for (size_t i = 0; i < _dataset.size(); ++i) {

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

    bool DatasetManipulator::exportYAMLDataset(std::vector<Pose> _dataset, int _gripper_type, std::string _prefix,
                                               std::string _file_name_with_path) {

        std::ofstream file;
        std::stringstream toFile, dof_ss, parameters_ss;


        if (_dataset.empty()) {
            output_string_ = "Cannot export an empty dataset.";
            return false;
        }

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
                   "    type: " << _gripper_type << "\n" <<
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

    bool DatasetManipulator::exportJSONDataset(std::vector<Pose> _dataset, std::string _prefix,
                                               std::string _file_name_with_path) {

        std::string name_tag;
        json_pose_arr_.clear();

        if (_dataset.empty()) {
            output_string_ = "Cannot export an empty dataset.";
            return false;
        }

        for (size_t i = 0; i < _dataset.size(); ++i) { // TODO: REMOVE: just to test

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

    bool DatasetManipulator::exportYAMLDataset(std::vector<Pose> _dataset, std::string _prefix,
                                               std::string _file_name_with_path) {

        std::ofstream file;
        std::stringstream toFile, dof_ss, parameters_ss;

        if (_dataset.empty()) {
            output_string_ = "Cannot export an empty dataset.";
            return false;
        }

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
}