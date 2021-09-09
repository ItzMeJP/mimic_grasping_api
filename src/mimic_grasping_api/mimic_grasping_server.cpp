#include "mimic_grasping_server.h"

namespace mimic_grasping {

    MimicGraspingServer::MimicGraspingServer(){
    }

    MimicGraspingServer::MimicGraspingServer(std::string _profile){
        setProfile(_profile);
    }

    MimicGraspingServer::~MimicGraspingServer(){


    }

    void MimicGraspingServer::setProfile(std::string _profile){
        profile_ = _profile;
    }

    std::string MimicGraspingServer::getProfile() {
        return profile_;
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

    bool MimicGraspingServer::start() {

        if(!load() || !init())
            return false;

        while(!stop_) {
            if(!spin())
                return false;
             }

        if(!stop())
            return false;

        return true;
    }

    bool MimicGraspingServer::stop(){
        if(!closeInterfaces())
            return false;

        if(!applyTransformation(obj_pose_arr_,tool_pose_arr_,tool_pose_wrt_obj_frame_arr_)) {
            output_string_ = getDatasetManipulatorOutputSTR();
            return false;
        }
        return true;
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

        if(profile_.empty()){
            std::cout << "Profile empty. Setting DEFAULT" << std::endl;
            profile_ = "default";
        }

        if(!loadFirmwareInterfaceConfigFile(root_folder_path_ + config_folder_dir_ + "/" + profile_ + tool_firmware_file_ )) {
            output_string_ = getToolFirmwareOutputSTR();
            return false;
        }

        if(!loadTransformationMatrix(root_folder_path_ + config_folder_dir_ + "/" + profile_ + matrix_file_)){
            output_string_ = getDatasetManipulatorOutputSTR();
            return false;
        }

        if(!loadLocalizationConfigFile(root_folder_path_ + config_folder_dir_ + "/" + profile_ + localization_file_)
        || !setLocalizationScriptsFolderPath(root_folder_path_ + scripts_folder_dir_ )
        || !setLocalizationConfigsFolderPath(root_folder_path_ + config_folder_dir_ + "/" + profile_)){
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
            DEBUG_MSG (output_string_);
            requestObjectLocalization();
        }

        return true;
    }

    bool MimicGraspingServer::exportDatasets() {

        struct stat info;

        std::string path_name = root_folder_path_+"/outputs/"+ profile_;
        char* char_arr = &path_name[0];

        if( stat( char_arr, &info ) != 0 ) {
            DEBUG_MSG("Cannot access " << char_arr << ". Creating it to export dataset.");
            std::filesystem::create_directory(path_name);
        }
        /*
        else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows
            printf( "%s is a directory\n", char_arr );
        else
            printf( "%s is no directory\n", char_arr );
        */

        int gripper_type_label;
        if(getGripperType() == GRIPPER_TYPE::SINGLE_SUCTION_CUP)
            gripper_type_label = GRIPPER_ID::SCHMALZ_SINGLE_RECT_X_SUCTION;
        else if(getGripperType()  == GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER)
            gripper_type_label = GRIPPER_ID::FESTO_2F_HGPC_16_A;


        if(
                !saveDataset(tool_pose_arr_, gripper_type_label, "candidate_", path_name +"/raw_grasping_poses.yaml",EXPORT_EXTENSION::YAML) ||
                !saveDataset(tool_pose_arr_, gripper_type_label, "candidate_", path_name +"/raw_grasping_poses.json",EXPORT_EXTENSION::JSON) ||
                !saveDataset(tool_pose_wrt_obj_frame_arr_, gripper_type_label, "candidate_", path_name +"/grasping_poses.yaml",EXPORT_EXTENSION::YAML) ||
                !saveDataset(tool_pose_wrt_obj_frame_arr_, gripper_type_label, "candidate_", path_name +"/grasping_poses.json",EXPORT_EXTENSION::JSON) ||
                !saveDataset(obj_pose_arr_, "object_pose_", path_name +"/object_poses.yaml",EXPORT_EXTENSION::YAML) ||
                !saveDataset(obj_pose_arr_, "object_pose_", path_name +"/object_poses.json",EXPORT_EXTENSION::JSON)
                ){

            output_string_ = getDatasetManipulatorOutputSTR();
            return false;
        }

        return true;
    }

    std::vector<Pose> MimicGraspingServer::getDataset(int _dataset_type){

        switch (_dataset_type) {
            case DATASET_TYPE::TOOL_POSES_WRT_SRC:
                return tool_pose_arr_;
            case DATASET_TYPE::TOOL_POSES_WRT_OBJ:
                return tool_pose_wrt_obj_frame_arr_;
            case DATASET_TYPE::OBJ_POSES_WRT_SRC:
                return obj_pose_arr_;
        }

    }

    void MimicGraspingServer::clearDataset(){
        obj_pose_arr_.clear();
        tool_pose_arr_.clear();
    }

    bool MimicGraspingServer::spin(){

        if(!firmware_spinner_sleep(2000)){ // the serial reader is slow...
            output_string_ = getToolFirmwareOutputSTR();
            while(stopToolLocalization()!=true){};
            while(stopObjLocalization()!=true){};
            output_string_  =  "Closing interfaces since firmware communication is lost." ;
            return false;
        }

        if(!object_localization_spinner_sleep(150)){
            output_string_ = getLocalizationOutputSTR();
            while(stopToolCommunication()!=true){}
            while(stopToolLocalization()!=true){};
            output_string_  = "Closing interfaces since object localization communication is lost." ;
            return false;
        }

        if(!tool_localization_spinner_sleep(150)){
            output_string_ = getLocalizationOutputSTR();
            while(stopToolCommunication()!=true){};
            while(stopObjLocalization()!=true){};
            output_string_  = "Closing interfaces since tool localization communication is lost." ;
            return false;
        }

        current_msg_ = received_msg_;
        convertMsgToCode(current_msg_,current_code_);

        output_string_ = current_msg_;
        DEBUG_MSG( output_string_ );

        if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING && !one_shoot_estimation_)
        {
            output_string_ = "Save pose request received.";
            DEBUG_MSG ( output_string_ );

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
                DEBUG_MSG( "" << current_tool_pose_.getName() );
                //tool_pose_arr_.push_back(current_tool_pose_);
                DEBUG_MSG( "Tool data size " << tool_pose_arr_.size() );
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
                DEBUG_MSG( "" << current_tool_pose_.getName() );
                //tool_pose_arr_.push_back(current_tool_pose_);
                DEBUG_MSG( "Tool data size " << tool_pose_arr_.size() );
                sendSuccessMsg();
            }

            else{
                sendErrorMsg();
                return false;
            }

        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING && !one_shoot_estimation_){
            output_string_ = "Remove last saved pose request received.";
            DEBUG_MSG( output_string_ );
            obj_pose_arr_.erase(obj_pose_arr_.end()); // because of the firmware state machine, this condition only happen after a success stock
            tool_pose_arr_.erase(tool_pose_arr_.end());
            DEBUG_MSG( "New object dataset size: " << obj_pose_arr_.size() );
            DEBUG_MSG( "New tool dataset size: " << tool_pose_arr_.size() );

            sendSuccessMsg();
        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING && one_shoot_estimation_){
            output_string_ = "Remove last save pose request received. ONE_SHOOT method.";
            DEBUG_MSG( output_string_ );
            tool_pose_arr_.erase(tool_pose_arr_.end());
            DEBUG_MSG( "New object dataset size [ONE_SHOOT mode ON]: " << obj_pose_arr_.size() );
            DEBUG_MSG( "New tool dataset size: " << tool_pose_arr_.size() );

            sendSuccessMsg();
        }

        return true;
    }

    bool MimicGraspingServer::closeInterfaces(){

        DEBUG_MSG( "Closing interfaces...");
        DEBUG_MSG( "Closing Tool Localization...");
        //while(stopToolLocalization()!=true){};
        stopToolLocalization();
        DEBUG_MSG( "Closing Object Localization...");
        //while(stopObjLocalization()!=true){};
        stopObjLocalization();
        sleep(1);
        DEBUG_MSG(  "Closing Tool Communication..." );
        //while(stopToolCommunication()!=true){}
        stopToolCommunication();

        return true; //TODO: treat possible errors
    };


    bool MimicGraspingServer::requestObjectLocalization(){

        setObjLocalizationTarget(root_folder_path_+"/models/single_side_bracket.ply");
        if(requestObjPose(current_obj_pose_)) {
            DEBUG_MSG( "" << current_obj_pose_.getName() );
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

    void MimicGraspingServer::request_stop(){
        stop_ = true;
    }

    std::string MimicGraspingServer::getOutputSTR(){
        return output_string_;
    }


}//end namespace