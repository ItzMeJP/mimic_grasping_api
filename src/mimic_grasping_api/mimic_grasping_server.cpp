#include "mimic_grasping_server.h"

namespace mimic_grasping {

    MimicGraspingServer::MimicGraspingServer(){
    }

    MimicGraspingServer::MimicGraspingServer(std::string _profile){
        setProfile(_profile);
    }

    MimicGraspingServer::~MimicGraspingServer(){
        //stop();
    }

    void MimicGraspingServer::setProfile(std::string _profile){
        profile_ = _profile;
    }

    std::string MimicGraspingServer::getProfile() {
        return profile_;
    }

    bool MimicGraspingServer::buildProfileList(){

        profile_list_.clear();
        for (const auto & entry : std::filesystem::directory_iterator(profile_folder_path_))
        {
            //std::cout << entry.path() << std::endl;
            profile_list_.push_back(entry.path().filename());
        }

        return true;
    }

    bool MimicGraspingServer::getProfileListAt(int _index, std::string &_outputStr){
        if(profile_list_.empty())
            buildProfileList();
        if(profile_list_.empty()){
            error_string_ = "None profile found at: " + profile_folder_path_;
            DEBUG_MSG(error_string_);
            return false;
        }
        _outputStr = profile_list_.at(_index);
        return true;
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

        if(!loadFirmwareInterfaceConfigFile(root_folder_path_ + profile_folder_path_ + tool_firmware_file_ ) ||
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
                saveFirmwareInterfaceConfigFile(root_folder_path_ + profile_folder_path_ + "/OutputTest.json");

        }

    }*/

    bool MimicGraspingServer::start() {

        if(!setup() || !load()){
            stop();
            return false;
        }

        if(!init()){
            stop();
            return false;
        }

        while(!stop_) {
            running_ = true;
            if (!spin()) {
                stop();
                running_ = false;
                return false;
            }
        }

        stop();
        running_ = false;

/*
        if(!applyTransformation(obj_pose_arr_,tool_pose_arr_,tool_pose_wrt_obj_frame_arr_)) {
            output_string_ = getDatasetManipulatorOutputSTR();
        }
*/

        return true;
    }

    bool MimicGraspingServer::stop(){
        output_string_ = "Detaching mimic_grasping api...";
        DEBUG_MSG( output_string_);

        if(!closeInterfaces())
            return false;

        if(plugin_list_is_loaded_) {
            clearPluginInstances(); // TODO: verify possible errors...
            plugin_list_is_loaded_ = false;
        }

        return true;
    }

    bool MimicGraspingServer::setup(){

        env_root_folder_path =  getenv("MIMIC_GRASPING_SERVER_ROOT");
        if(env_root_folder_path == NULL) {
            //output_string_ = "The environment variable $MIMIC_GRASPING_SERVER_ROOT is not defined.";
            error_string_ = "The environment variable $MIMIC_GRASPING_SERVER_ROOT is not defined.";
            DEBUG_MSG(error_string_);
            return false;
        }
        stop_ = false;

        root_folder_path_ = std::string(env_root_folder_path);

        if(profile_.empty()){
            output_string_ ="Profile empty. Setting DEFAULT";
            profile_ = "default";
        }
        else{
            output_string_ = "Selected Profile: " + profile_;
        }

        output_export_path_ = root_folder_path_+"/outputs/"+ profile_;
        config_folder_path_ = root_folder_path_ + config_folder_dir_ ;
        profile_folder_path_ = config_folder_path_+ "/" + profile_;
        script_folder_path_ = root_folder_path_ + scripts_folder_dir_;
        plugins_folder_path_ = root_folder_path_ + plugins_folder_dir_;

        if(!std::filesystem::exists(profile_folder_path_)){
            output_string_ ="Profile \"" + profile_ + "\" does not exist.";
            DEBUG_MSG(output_string_);
            generateProfileDirectoryTemplate();
        }

        buildProfileList();

//        DEBUG_MSG("Current output export file defined: " << getOutputExportPath());
        output_string_ = "Setup process has been completed.";

        return true;

    }

    bool MimicGraspingServer::load(){

        if(!loadFirmwareInterfaceConfigFile( profile_folder_path_ + tool_firmware_file_ )) {
            //output_string_ = getToolFirmwareOutputSTR();
            error_string_ = "Tool firmware error: " + getToolFirmwareOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }

        if(!loadTransformationMatrix(profile_folder_path_ + matrix_file_)){
            //output_string_ = getDatasetManipulatorOutputSTR();
            error_string_ = "Dataset manipulator error: " + getDatasetManipulatorOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }

        if(!loadLocalizationConfigFile(profile_folder_path_ + localization_file_)
        || !setLocalizationScriptsFolderPath( script_folder_path_ )
        || !setLocalizationConfigsFolderPath(profile_folder_path_)){
            //output_string_ = getLocalizationOutputSTR();
            error_string_ = "Localization error: " + getLocalizationInterfaceOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }

        if(!loadDynamicPlugins( plugins_folder_path_+"/",true)){ // TODO: load config file for plugin
            //output_string_ = getPluginManagementOutputMsg();
            error_string_ = "Plugin management error: " + getPluginManagementOutputMsg();
            DEBUG_MSG(error_string_);
            plugin_list_is_loaded_ = false;
            return false;
        }
        plugin_list_is_loaded_ = true;

        output_string_ = "Loading process has been completed.";
        return true;
    }

    bool MimicGraspingServer::init(){

        clearDataset();
        last_current_code_ = -999;

        output_string_ = "Initializing tool firmware communication...";
        if(!initToolFirmware(false)) {
            //output_string_ = getToolFirmwareOutputSTR();
            output_string_ += "\n-- Failed to initialize tool firmware communication.";
            error_string_ = "Failed to initialize tool firmware communication. " + getToolFirmwareOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }
        initialized_firmware_communication_ = true;

        output_string_ = "Initializing object localization plugin...";
        if(!initObjLocalization() ){
            //output_string_ = getLocalizationInterfaceOutputSTR();
            output_string_ += "\n-- Failed to initialize object localization.";
            error_string_ = "Failed to initialize object localization. " + getLocalizationInterfaceOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }
        initialized_obj_localization_ = true;

        output_string_ = "Initializing tool localization plugin...";
        if(!initToolLocalization() ){
            //output_string_ = getLocalizationInterfaceOutputSTR();
            output_string_ += "\n-- Failed to initialize tool localization.";
            error_string_ = "Failed to initialize tool localization. " + getLocalizationInterfaceOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }
        initialized_tool_localization_ = true;

        if(isOneShoot()) {
            output_string_ = "ONE_SHOOT method active. Running object localization...";
            DEBUG_MSG (output_string_);
            if (!requestObjectLocalization()) {
                //output_string_ = "Failed to localize the object.";
                error_string_ = "Failed to localize the object.";
                DEBUG_MSG(error_string_);
                return false;
            }
            output_string_ = "Object detected.";

        }

        if (!setGripperType()) { //set the running mode in firmware state machine
            //output_string_ = getToolFirmwareOutputSTR();
            error_string_ = "Tool firmware error: " + getToolFirmwareOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }

        output_string_ = "Initialization has been completed. Teaching process started.";

        return true;
    }

    bool MimicGraspingServer::exportDatasets() {

        DEBUG_MSG("Current output export file defined: " << getOutputExportPath());

        if(getOutputExportPath().empty()){
            error_string_ = "Output path is not defined. The system should be, at least, loaded to export the dataset.";
            DEBUG_MSG(error_string_);
            return false;
        }

        struct stat info;
        char* char_arr = &output_export_path_[0];

        if( stat( char_arr, &info ) != 0 ) {
            DEBUG_MSG("Cannot access " << char_arr << ". Creating it to export dataset.");
            std::filesystem::create_directory(output_export_path_);
        }
        /*
        else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows
            printf( "%s is a directory\n", char_arr );
        else
            printf( "%s is no directory\n", char_arr );
        */

        int gripper_type_label;
        if(getGripperType() == GRIPPER_TYPE::SINGLE_SUCTION_CUP)
            gripper_type_label = GRIPPER_ID::SCHMALZ_FOAM_SUCTION_CUP_FMSW_N10_76x22;
        else if(getGripperType()  == GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER)
            gripper_type_label = GRIPPER_ID::FESTO_2F_HGPC_16_A_30;


        if(
                !saveDataset(tool_pose_arr_, gripper_type_label, "candidate_", output_export_path_ +"/raw_grasping_poses.yaml",EXPORT_EXTENSION::YAML) ||
                !saveDataset(tool_pose_arr_, gripper_type_label, "candidate_", output_export_path_ +"/raw_grasping_poses.json",EXPORT_EXTENSION::JSON) ||
                !saveDataset(tool_pose_wrt_obj_frame_arr_, gripper_type_label, "candidate_", output_export_path_ +"/grasping_poses.yaml",EXPORT_EXTENSION::YAML) ||
                !saveDataset(tool_pose_wrt_obj_frame_arr_, gripper_type_label, "candidate_", output_export_path_ +"/grasping_poses.json",EXPORT_EXTENSION::JSON) ||
                !saveDataset(obj_pose_arr_, "object_pose_", output_export_path_ +"/object_poses.yaml",EXPORT_EXTENSION::YAML) ||
                !saveDataset(obj_pose_arr_, "object_pose_", output_export_path_ +"/object_poses.json",EXPORT_EXTENSION::JSON)
                ){

            //output_string_ = getDatasetManipulatorOutputSTR();
            error_string_ = "Dataset manipulator error: " + getDatasetManipulatorOutputSTR();
            DEBUG_MSG(error_string_);
            return false;
        }

        return true;
    }

    std::string MimicGraspingServer::getOutputExportPath() {
        return output_export_path_;
    }

    std::string MimicGraspingServer::getPluginsFolderPath(){
        return plugins_folder_path_;
    }
    std::string MimicGraspingServer::getProfileFolderPath(){
        return profile_folder_path_;
    }
    std::string MimicGraspingServer::getConfigFolderPath(){
        return config_folder_path_;
    }
    std::string MimicGraspingServer::getScriptFolderPath(){
        return script_folder_path_;
    }

    std::vector<Pose> MimicGraspingServer::getDataset(int _dataset_type){

        switch (_dataset_type) {
            case DATASET_TYPE::TOOL_POSES_WRT_SRC:
                return tool_pose_arr_;
            case DATASET_TYPE::TOOL_POSES_WRT_OBJ:
                return tool_pose_wrt_obj_frame_arr_;
            case DATASET_TYPE::OBJ_POSES_WRT_SRC:
                return obj_pose_arr_;
            default:
                assert(false);
        }

    }

    Pose MimicGraspingServer::getDataset(int _dataset_type, int _index){

        if(_index >= 0){
            switch (_dataset_type) {
                case DATASET_TYPE::TOOL_POSES_WRT_SRC:
                    assert(! (_index > tool_pose_arr_.size()));
                    return tool_pose_arr_.at(_index);

                case DATASET_TYPE::TOOL_POSES_WRT_OBJ:
                    assert(! (_index > tool_pose_wrt_obj_frame_arr_.size()));
                    return tool_pose_wrt_obj_frame_arr_.at(_index);

                case DATASET_TYPE::OBJ_POSES_WRT_SRC:
                    assert(! (_index > obj_pose_arr_.size()));
                    return obj_pose_arr_.at(_index);
                default:
                    assert(false);
            }
        }
        else{
            switch (_dataset_type) {
                case DATASET_TYPE::TOOL_POSES_WRT_SRC:
                    return tool_pose_arr_.at(tool_pose_arr_.size()-1);

                case DATASET_TYPE::TOOL_POSES_WRT_OBJ:
                    return tool_pose_wrt_obj_frame_arr_.at(tool_pose_wrt_obj_frame_arr_.size()-1);

                case DATASET_TYPE::OBJ_POSES_WRT_SRC:
                    return obj_pose_arr_.at(obj_pose_arr_.size()-1);
                default:
                    assert(false);
            }
        }

    }

    void MimicGraspingServer::clearDataset(){
        obj_pose_arr_.clear();
        tool_pose_arr_.clear();
        tool_pose_wrt_obj_frame_arr_.clear();
    }

    bool MimicGraspingServer::spin(){


        if(!firmware_spinner_sleep(500)){ // the serial reader is slow...
            error_string_ = "Tool firmware spin error: " + getToolFirmwareOutputSTR();
            initialized_firmware_communication_ = false;
            DEBUG_MSG(error_string_);
            stop();
            output_string_  =  "Closing interfaces since firmware communication is lost." ;
            return false;
        }

        if(!object_localization_spinner_sleep(150)){
            error_string_ = "Object localization spin error: " + getLocalizationInterfaceOutputSTR();
            DEBUG_MSG(error_string_);
            initialized_obj_localization_ = false;
            stop();
            output_string_  = "Closing interfaces since object localization communication is lost." ;
            return false;
        }

        if(!tool_localization_spinner_sleep(150)){
            error_string_ = "Tool localization spin error: " + getLocalizationInterfaceOutputSTR();
            DEBUG_MSG(error_string_);
            initialized_tool_localization_ = false;
            stop();
            output_string_  = "Closing interfaces since tool localization communication is lost." ;
            return false;
        }

        current_msg_ = received_msg_;
        convertMsgToCode(current_msg_,current_code_);

        if(current_code_ == last_current_code_){
            return true;
        }
        last_current_code_ = current_code_;

        DEBUG_MSG( current_msg_ );

        if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING && !isOneShoot())
        {
            bool break_it = false; //TODO: change this control variable by using the LocalizationBase::ABORTED from LocalizationBase class.
            output_string_ = "Save pose request received. Running object localization...";
            DEBUG_MSG ( output_string_ );
            if(requestObjectLocalization()) {
                DEBUG_MSG( "" << current_obj_pose_.getName() );
                //obj_pose_arr_.push_back(current_obj_pose_);
                DEBUG_MSG( "Object data size " << obj_pose_arr_.size() );
            }

            else{
                sendErrorMsg();
                output_string_ = "Failed to localize the object.";
                break_it = true;
            }

            if(!break_it) {
                output_string_ = "Running tool localization...";
                if (requestToolLocalization()) {
                    DEBUG_MSG("" << current_tool_pose_.getName());
                    //tool_pose_arr_.push_back(current_tool_pose_);
                    DEBUG_MSG("Tool data size " << tool_pose_arr_.size());
                    sendSuccessMsg();
                } else {
                    output_string_ = "Failed to localize the tool.";
                    obj_pose_arr_.pop_back();
                    sendErrorMsg();
                    //return false;
                }

                output_string_ +=
                        "Acquisition saved with success. Grasping poses dataset size: " + tool_pose_arr_.size();
            }
        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING && isOneShoot())
        {
            output_string_ = "Save pose request received. Running tool localization...";
            if(requestToolLocalization()) {
                DEBUG_MSG( "" << current_tool_pose_.getName() );
                //tool_pose_arr_.push_back(current_tool_pose_);
                DEBUG_MSG( "Tool data size " << tool_pose_arr_.size() );
                sendSuccessMsg();
            }

            else{
                output_string_ = "Failed to localize the tool.";
                output_string_ = getLocalizationInterfaceOutputSTR();
                sendErrorMsg();
                //return false;
            }

            output_string_ += "Acquisition saved with success. Grasping poses dataset size: " + tool_pose_arr_.size();

        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING && !isOneShoot()){
            output_string_ = "Remove last saved pose request received.";
            DEBUG_MSG( output_string_ );

            obj_pose_arr_.erase(obj_pose_arr_.end()); // because of the firmware state machine, this condition only happen after a success stock
            tool_pose_arr_.erase(tool_pose_arr_.end());
            tool_pose_wrt_obj_frame_arr_.erase(tool_pose_wrt_obj_frame_arr_.end());

            DEBUG_MSG( "New object dataset size: " << obj_pose_arr_.size() );
            DEBUG_MSG( "New tool dataset size: " << tool_pose_arr_.size() );
            sendSuccessMsg();
            output_string_ += "Removed last grasping pose. Grasping poses dataset size: " + tool_pose_arr_.size();

        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING && isOneShoot()){
            output_string_ = "Remove last save pose request received.";
            DEBUG_MSG( output_string_ );
            tool_pose_arr_.erase(tool_pose_arr_.end());
            tool_pose_wrt_obj_frame_arr_.erase(tool_pose_wrt_obj_frame_arr_.end());

            DEBUG_MSG( "New object dataset size [ONE_SHOOT mode ON]: " << obj_pose_arr_.size() );
            DEBUG_MSG( "New tool dataset size: " << tool_pose_arr_.size() );

            sendSuccessMsg();
            output_string_ += "Removed last grasping pose. Grasping poses dataset size: " + tool_pose_arr_.size();

        }
        //output_string_ += output_string_==""?current_msg_:"\n" + current_msg_;
        return true;
    }

    bool MimicGraspingServer::closeInterfaces(){

        if(initialized_tool_localization_){
            DEBUG_MSG( "Closing Tool Localization...");
            output_string_ = "Closing Tool Localization...";
            //while(stopToolLocalization()!=true){};
            stopToolLocalization();
            initialized_tool_localization_ = false;
        }
        //sleep(1);

        if(initialized_obj_localization_) {
            DEBUG_MSG( "Closing Object Localization...");
            output_string_ = "Closing Object Localization...";
            //while (stopObjLocalization() != true) {};
            stopObjLocalization();
            initialized_obj_localization_ = false;
        }
        //sleep(1);

        if(initialized_firmware_communication_) {
            DEBUG_MSG(  "Closing Tool Communication..." );
            output_string_ = "Closing Tool Communication...";
            //while (stopToolCommunication() != true) {}
            stopToolCommunication();
            initialized_firmware_communication_ = false;
        }
        //sleep(1);

        return true; //TODO: treat possible errors
    };


    bool MimicGraspingServer::requestObjectLocalization(){

        if(requestObjPose(current_obj_pose_)) {
            DEBUG_MSG( "################################################ ");
            DEBUG_MSG( "Object Name: " << current_obj_pose_.getName() );
            DEBUG_MSG( "Object Parent Frame: " << current_obj_pose_.getParentName() );
            DEBUG_MSG( "Object Position [x, y, z][m]: " << "[" << current_obj_pose_.getPosition().x() << ", " << current_obj_pose_.getPosition().y() << ", " << current_obj_pose_.getPosition().z() << "]" );
            DEBUG_MSG( "Object Orientation [x, y, z, w]: " << "[" << current_obj_pose_.getQuaternionOrientation().x() << ", " << current_obj_pose_.getQuaternionOrientation().y() << ", " << current_obj_pose_.getQuaternionOrientation().z() << ", " << current_obj_pose_.getQuaternionOrientation().w() << "]" );
            DEBUG_MSG( "Object Orientation Euler Accumulative (Z-Y-X) [roll, pitch, yaw][rad]: " << "[" << current_obj_pose_.getRPYOrientationZYXOrder().x() << ", " << current_obj_pose_.getRPYOrientationZYXOrder().y() << ", " << current_obj_pose_.getRPYOrientationZYXOrder().z() << "]" );
            DEBUG_MSG( "################################################ ");
            obj_pose_arr_.push_back(current_obj_pose_);
            return true;
        }
        else
            return false;
    }

    bool MimicGraspingServer::requestToolLocalization(){

        if(requestToolPose(current_tool_pose_)) {
            DEBUG_MSG( "################################################ ");
            DEBUG_MSG( "Grasping Name: " << current_tool_pose_.getName() );
            DEBUG_MSG( "Grasping Parent Frame: " << current_tool_pose_.getParentName() );
            DEBUG_MSG( "Grasping Position [x, y, z][m]: " << "[" << current_tool_pose_.getPosition().x() << ", " << current_tool_pose_.getPosition().y() << ", " << current_tool_pose_.getPosition().z() << "]" );
            DEBUG_MSG( "Grasping Orientation [x, y, z, w]: " << "[" << current_tool_pose_.getQuaternionOrientation().x() << ", " << current_tool_pose_.getQuaternionOrientation().y() << ", " << current_tool_pose_.getQuaternionOrientation().z() << ", " << current_tool_pose_.getQuaternionOrientation().w() << "]" );
            DEBUG_MSG( "Grasping Orientation Euler Accumulative (Z-Y-X) [roll, pitch, yaw][rad]: " << "[" << current_tool_pose_.getRPYOrientationZYXOrder().x() << ", " << current_tool_pose_.getRPYOrientationZYXOrder().y() << ", " << current_tool_pose_.getRPYOrientationZYXOrder().z() << "]" );
            DEBUG_MSG( "################################################ ");
            tool_pose_arr_.push_back(current_tool_pose_);

            Pose aux;
            if(!applyTransformation(obj_pose_arr_.back(),tool_pose_arr_.back(),aux)) {
                output_string_ = getDatasetManipulatorOutputSTR();
                return false;
            }
            DEBUG_MSG( "################################################ ");
            DEBUG_MSG( "Grasping Name: " << aux.getName() );
            DEBUG_MSG( "Grasping Parent Frame: " << aux.getParentName() );
            DEBUG_MSG( "Grasping Position [x, y, z][m]: " << "[" << aux.getPosition().x() << ", " << aux.getPosition().y() << ", " << aux.getPosition().z() << "]" );
            DEBUG_MSG( "Grasping Orientation [x, y, z, w]: " << "[" << aux.getQuaternionOrientation().x() << ", " << aux.getQuaternionOrientation().y() << ", " << aux.getQuaternionOrientation().z() << ", " << aux.getQuaternionOrientation().w() << "]" );
            DEBUG_MSG( "Grasping Orientation Euler Accumulative (Z-Y-X) [roll, pitch, yaw][rad]: " << "[" << aux.getRPYOrientationZYXOrder().x() << ", " << aux.getRPYOrientationZYXOrder().y() << ", " << aux.getRPYOrientationZYXOrder().z() << "]" );
            DEBUG_MSG( "################################################ ");

            tool_pose_wrt_obj_frame_arr_.push_back(aux);

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
        DEBUG_MSG("Stop request received.");
        while(running_){};
    }

    std::string MimicGraspingServer::getOutputSTR(){
        return output_string_;
    }

    std::string MimicGraspingServer::getErrorStr(){
        return error_string_;
    }

    int MimicGraspingServer::datasetSize(){
        return tool_pose_wrt_obj_frame_arr_.size();
    }

    bool MimicGraspingServer::isDatasetEmpty(){
        return tool_pose_wrt_obj_frame_arr_.size()==0?true:false;
    }

    bool MimicGraspingServer::generateProfileDirectoryTemplate(){
        output_string_ = "Creating a template profile directory...";
        DEBUG_MSG(output_string_);
        mkdir(profile_folder_path_.c_str(),0777);
        saveFirmwareInterfaceConfigFile(profile_folder_path_  + tool_firmware_file_);
        saveLocalizationConfigFile(profile_folder_path_  + localization_file_);
        saveTransformationMatrix(profile_folder_path_ + matrix_file_);
        return true;
    }



}//end namespace