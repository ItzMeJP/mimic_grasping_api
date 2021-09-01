#include "mimic_grasping_server.h"

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

        int aux;
        while(!stop_) {
            if(!spin())
                return;
            std::cout << "Continue?[0|1]" << std::endl;
            std::cin >> aux;
            stop_=(aux==0?true:false);
        }

        closeInterfaces();

        int gripper_type_label;
        if(getGripperType() == GRIPPER_TYPE::SINGLE_SUCTION_CUP)
            gripper_type_label = GRIPPER_ID::SCHMALZ_SINGLE_RECT_X_SUCTION;
        else if(getGripperType()  == GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER)
            gripper_type_label = GRIPPER_ID::FESTO_2F_HGPC_16_A;

        saveDataset(tool_pose_arr_, gripper_type_label, "candidate_", root_folder_path_+"/outputs/grasping_poses.yaml",EXPORT_EXTENSION::YAML); // TODO: REMOVE: just to test
        saveDataset(tool_pose_arr_, gripper_type_label, "candidate_", root_folder_path_+"/outputs/grasping_poses.json",EXPORT_EXTENSION::JSON); // TODO: REMOVE: just to test
        saveDataset(obj_pose_arr_, "object_pose_", root_folder_path_+"/outputs/object_poses.yaml",EXPORT_EXTENSION::YAML); // TODO: REMOVE: just to test
        saveDataset(obj_pose_arr_, "object_pose_", root_folder_path_+"/outputs/object_poses.json",EXPORT_EXTENSION::JSON); // TODO: REMOVE: just to test

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

        if(!loadFirmwareInterfaceConfigFile(root_folder_path_ + config_folder_dir_ + tool_firmware_file_ )) {
            output_string_ = getToolFirmwareOutputSTR();
            return false;
        }
        if(!loadLocalizationConfigFile(root_folder_path_ + config_folder_dir_ + localization_file_)
        || !setLocalizationScriptsFolderPath(root_folder_path_ + scripts_folder_dir_ )
        || !setLocalizationConfigsFolderPath(root_folder_path_ + config_folder_dir_)){
            output_string_ = getLocalizationOutputSTR();
        }

        if(!loadDynamicPlugins(root_folder_path_ + plugins_folder_dir_ +"/",true)){ // TODO: load config file for plugin
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

        if(!initObjLocalization() || !initToolLocalization() ){
            output_string_ = getLocalizationOutputSTR();
            return false;
        }

        if(one_shoot_estimation_ == true)
        {
            output_string_ = "ONE_SHOOT method active.";
            std::cout << output_string_ << std::endl;
            requestObjectLocalization();
        }

        return true;
    }

    void MimicGraspingServer::clearDataset(){
        obj_pose_arr_.clear();
        tool_pose_arr_.clear();
    }

    bool MimicGraspingServer::spin(){

        if(!firmware_spinner_sleep(2000)){ // the serial reader is slow...
            output_string_ = getToolFirmwareOutputSTR();
            while(stopToolLocalization()!=true){}; // TODO: double free error... the interruption does not the code continue
            while(stopObjLocalization()!=true){};
            //closeInterfaces();
            std::cout<< "continuei 0" << std::endl;
            return false;
        }

        if(!object_localization_spinner_sleep(150)){
            output_string_ = getLocalizationOutputSTR();
            while(stopToolCommunication()!=true){}
            while(stopToolLocalization()!=true){};

           //closeInterfaces();
            return false;
        }

        if(!tool_localization_spinner_sleep(150)){
            output_string_ = getLocalizationOutputSTR();
            while(stopToolCommunication()!=true){};
            while(stopObjLocalization()!=true){};

            //closeInterfaces();
            return false;
        }

        current_msg_ = received_msg_;
        convertMsgToCode(current_msg_,current_code_);

        output_string_ = current_msg_;
        std::cout << output_string_ << std::endl;

        if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING && !one_shoot_estimation_)
        {
            output_string_ = "Save pose request received.";
            std::cout << output_string_ << std::endl;

            if(requestObjectLocalization()) {
                std::cout << "" << current_obj_pose_.getName() << std::endl;
                //obj_pose_arr_.push_back(current_obj_pose_);
                std::cout << "Object data size" << obj_pose_arr_.size() << std::endl;
            }

            else{
                sendErrorMsg();
                return false;
            }

            if(requestToolLocalization()) {
                std::cout << "" << current_tool_pose_.getName() << std::endl;
                //tool_pose_arr_.push_back(current_tool_pose_);
                std::cout << "Tool data size " << tool_pose_arr_.size() << std::endl;
                sendSuccessMsg();
            }

            else{
                obj_pose_arr_.pop_back();
                sendErrorMsg();
                return false;
            }
        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING && one_shoot_estimation_)
        {
            output_string_ = "Save pose request received. ONE_SHOOT method.";
            std::cout << output_string_ << std::endl;

            if(requestToolLocalization()) {
                std::cout << "" << current_tool_pose_.getName() << std::endl;
                //tool_pose_arr_.push_back(current_tool_pose_);
                std::cout << "Tool data size " << tool_pose_arr_.size() << std::endl;
                sendSuccessMsg();
            }

            else{
                sendErrorMsg();
                return false;
            }

        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING && !one_shoot_estimation_){
            output_string_ = "Remove last saved pose request received.";
            std::cout << output_string_ << std::endl;
            obj_pose_arr_.erase(obj_pose_arr_.end()); // because of the firmware state machine, this condition only happen after a success stock
            tool_pose_arr_.erase(tool_pose_arr_.end());
            std::cout << "New object dataset size: " << obj_pose_arr_.size() << std::endl;
            std::cout << "New tool dataset size: " << tool_pose_arr_.size() << std::endl;

            sendSuccessMsg();
        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING && one_shoot_estimation_){
            output_string_ = "Remove last save pose request received. ONE_SHOOT method.";
            std::cout << output_string_ << std::endl;
            tool_pose_arr_.erase(tool_pose_arr_.end());
            std::cout << "New object dataset size [ONE_SHOOT mode ON]: " << obj_pose_arr_.size() << std::endl;
            std::cout << "New tool dataset size: " << tool_pose_arr_.size() << std::endl;

            sendSuccessMsg();
        }

        return true;
    }

    bool MimicGraspingServer::closeInterfaces(){

        std::cout << "Closing interfaces..." << std::endl;
        std::cout << "Closing Object Localization..." << std::endl;
        std::cout << "Closing Tool Localization..." << std::endl;
        while(stopToolLocalization()!=true){}; // TODO: cant kill it...
        while(stopObjLocalization()!=true){};
        sleep(1);
        std::cout << "Closing Tool Communication..." << std::endl;
        while(stopToolCommunication()!=true){}




        std::cout << "fechou tudo" << std::endl;
        return true; //TODO: treat possible errors
    };


    bool MimicGraspingServer::requestObjectLocalization(){

        setObjLocalizationTarget(root_folder_path_+"/models/single_side_bracket.ply");
        if(requestObjPose(current_obj_pose_)) {
            std::cout << "" << current_obj_pose_.getName() << std::endl;
            obj_pose_arr_.push_back(current_obj_pose_);
            return true;
        }
        else
            return false;
    }

    bool MimicGraspingServer::requestToolLocalization(){

        setToolLocalizationTarget("candidate_");
        if(requestToolPose(current_tool_pose_)) {
            //std::cout << "" << current_tool_pose_.getName() << std::endl;
            tool_pose_arr_.push_back(current_tool_pose_);
            return true;
        }
        else
            return false;
    }

    int MimicGraspingServer::getCurrentStateCode(){
        return current_code_;
    }

    void MimicGraspingServer::stop(){
        stop_ = true;
    }

    std::string MimicGraspingServer::getOutputSTR(){
        return output_string_;
    }


}//end namespace