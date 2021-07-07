#include "mimic_grasping_server/mimic_grasping_server.h"

namespace mimic_grasping {

    MimicGraspingServer::MimicGraspingServer(){
    }

    MimicGraspingServer::~MimicGraspingServer(){

    }

    /*
    void MimicGraspingServer::start() {

        output_string_ = "Null";

        env_root_folder_path =  getenv("MIMIC_GRASPING_SERVER_ROOT");
        if(env_root_folder_path == NULL) {
            output_string_ = "The environment variable $MIMIC_GRASPING_SERVER_ROOT is not defined";
            return;
        }

        root_folder_path_ = std::string(env_root_folder_path);

        if(!loadFirmwareInterfaceConfigFile(root_folder_path_ + config_folder_path_ + tool_firmware_file_ ) ||
           !startToolCommunication(output_string_) ||
           !setGripperType(ToolFirmwareInterface::GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER))
            return;

        std::string command = "";
        for(;;) {

            std::cout << "Current msg: " << received_msg_ << std::endl;
            std::cin>>command;
            writeSerialCommand(command);

            if(command == "-1")
            {
                std::cout<< "Changing config. Setting baud rate" << std::endl;
                if(!setSerialConfig("/dev/ttyUSB0",115200,output_string_))
                    return;
            }
            if(command == "-99")
            {
                std::cout<< "Changing gripper type to TWO F" << std::endl;
                if(!setGripperType(GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER))
                    return;
            }
            if(command == "-100")
            {
                std::cout<< "Changing gripper type to SINGLE SUC" << std::endl;
                if(!setGripperType(GRIPPER_TYPE::SINGLE_SUCTION_CUP))
                    return;
            }
            if(command == "-103")
                resetFirmware();

            if(command == "-n")
                sendErrorMsg();

            if(command == "-s")
                sendSuccessMsg();

            if(command == "save")
                saveFirmwareInterfaceConfigFile(root_folder_path_ + config_folder_path_ + "/OutputTest.json");

        }

    }*/

    void MimicGraspingServer::start() {

        if(!load() || !init())
            return;

        // TODO: run and test localizations modules
        while(!stop_) {
            spin();
            if(!firmware_spinner_sleep(1000)){
                output_string_ = getToolFirmwareOutputSTR();
                break;
            }
            if(!localization_spinner_sleep(1000)){
                output_string_ = getLocalizationOutputSTR();
                break;
            }

        }

        //saveDataset(root_folder_path_+"/dataset_test.yaml",EXPORT_EXTENSION::YAML); // just to test

    }

    bool MimicGraspingServer::load(){

        output_string_ = "Null";

        env_root_folder_path =  getenv("MIMIC_GRASPING_SERVER_ROOT");
        if(env_root_folder_path == NULL) {
            output_string_ = "The environment variable $MIMIC_GRASPING_SERVER_ROOT is not defined.";
            return false;
        }
        stop_ = false;

        root_folder_path_ = std::string(env_root_folder_path);

        if(!loadFirmwareInterfaceConfigFile(root_folder_path_ + config_folder_path_ + tool_firmware_file_ )) {
            output_string_ = getToolFirmwareOutputSTR();
            return false;
        }
        if(!loadLocalizationConfigFile(root_folder_path_ + config_folder_path_ + localization_file_)){
            output_string_ = getLocalizationOutputSTR();
        }

        if(!loadDynamicPlugins(root_folder_path_ + plugins_folder_path_,true)){ // TODO: load config file for plugin
            output_string_ = getPluginManagementOutputMsg();
            return false;
        }

        output_string_ = "Loading process has been completed.";
        return true;
    }

    bool MimicGraspingServer::init(){

        clearDataset();

        if(!initToolFirmware()) {
            output_string_ = getToolFirmwareOutputSTR();
            return false;
        }

        if(!initToolLocalization(root_folder_path_ + scripts_folder_) ||
           !initObjLocalization(root_folder_path_ + scripts_folder_) ){
            output_string_ = getLocalizationOutputSTR();
            return false;
        }

        return true;
    }

    void MimicGraspingServer::clearDataset(){
        obj_pose_arr_.clear();
        tool_pose_arr_.clear();
    }

    bool MimicGraspingServer::spin(){

        current_msg_ = received_msg_;
        convertMsgToCode(current_msg_,current_code_);

        output_string_ = current_msg_;
        std::cout << output_string_ << std::endl;

        if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING)
        {
            output_string_ = "Save pose request received.";
            std::cout << output_string_ << std::endl;

            if(requestObjPose(current_obj_pose_)) {
                std::cout << "" << current_obj_pose_.getName() << std::endl;
                obj_pose_arr_.push_back(current_obj_pose_);
                std::cout << "Dataset size" << obj_pose_arr_.size() << std::endl; //TODO: continue here

                sendSuccessMsg();
            }

            else
                sendErrorMsg();
        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING){
            output_string_ = "Remove last save pose request received.";
            std::cout << output_string_ << std::endl;
            obj_pose_arr_.erase(obj_pose_arr_.end()); // because of the firmware state machine, this condition only happen after a success stock
            std::cout << "Dataset size" << obj_pose_arr_.size() << std::endl; //TODO: continue here
            sendSuccessMsg();
            //TODO
        }

        return true;
    }

    int MimicGraspingServer::getCurrentStateCode(){
        return current_code_;
    }

    void MimicGraspingServer::stop(){
        stop_ = true;
    }

    bool MimicGraspingServer::saveDataset(std::string _path, int _type){
        if(_type == EXPORT_EXTENSION::JSON)
            return exportJSONDataset(_path);
        else
            return exportYAMLDataset(_path);
    }

    bool MimicGraspingServer::exportJSONDataset(std::string _path) {

        return true;
    }

    bool MimicGraspingServer::exportYAMLDataset(std::string _path) {

        std::ofstream file;

        std::stringstream toFile, dof_ss, parameters_ss;
        std::string frame_case;

        std::vector<Pose> g;
        frame_case = "tcp_from_object";
        //g = tool_pose_arr_; // TODO: change to this ...
        g = obj_pose_arr_; // TODO: Just debug

        for (size_t i = 0; i < g.size(); ++i) {

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

            int gripper_type_label;
            if(current_gripper_type_==GRIPPER_TYPE::SINGLE_SUCTION_CUP)
                gripper_type_label = GRIPPER_ID::SCHMALZ_SINGLE_RECT_X_SUCTION;
            else if(current_gripper_type_==GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER)
                gripper_type_label = GRIPPER_ID::FESTO_2F_HGPC_16_A;


            toFile << "candidate_" << std::to_string(i) << ":\n" <<
                   "  method: " << "\n" <<
                   "    type: " << SYNTHESIS_METHOD::MIMIC_GRASPING << "\n" <<
                   "  gripper:" << "\n" <<
                   "    type: " <<  gripper_type_label << "\n" <<
                   "    parameters: " << parameters_ss.str() << "\n" <<
                   "    DOFs: " << dof_ss.str() << "\n" <<
                   "  parent_frame_id: " << g.at(i).getParentName() << "\n" <<
                   "  position:" << "\n" <<
                   "    x: " << g.at(i).getPosition().x() << "\n" <<
                   "    y: " << g.at(i).getPosition().y() << "\n" <<
                   "    z: " << g.at(i).getPosition().z() << "\n" <<
                   "  orientation:" << "\n" <<
                   "    x: " << g.at(i).getQuaternionOrientation().x() << "\n" <<
                   "    y: " << g.at(i).getQuaternionOrientation().y() << "\n" <<
                   "    z: " << g.at(i).getQuaternionOrientation().z() << "\n" <<
                   "    w: " << g.at(i).getQuaternionOrientation().w() << "\n";

        }

        file.open(_path);
        file << toFile.str();
        file.close();
        return true;

    }


    std::string MimicGraspingServer::getOutputSTR(){
        return output_string_;
    }
}//end namespace