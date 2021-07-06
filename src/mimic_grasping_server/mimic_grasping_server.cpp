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
            firmware_spinner_sleep(1000);
            obj_localization_spinner_sleep(1000);
        }

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


//TODO: select tool localization plugin e object localization plugin at a plugin_manager_interface.h


        return true;
    }

    bool MimicGraspingServer::init(){

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

    bool MimicGraspingServer::spin(){

        current_msg_ = received_msg_;
        convertMsgToCode(current_msg_,current_code_);

        output_string_ = current_msg_;
        std::cout << output_string_ << std::endl;

        if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING)
        {
            output_string_ = "Save pose request received.";
            std::cout << output_string_ << std::endl;
            sendSuccessMsg();
            //TODO
        }
        else if(current_code_ == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING){
            output_string_ = "Remove last save pose request received.";
            std::cout << output_string_ << std::endl;
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

    std::string MimicGraspingServer::getOutputSTR(){
        return output_string_;
    }
}//end namespace