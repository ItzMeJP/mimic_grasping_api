#include <mimic_grasping_server/mimic_grasping_server.h>

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

        output_string_ = "Null";

        env_root_folder_path =  getenv("MIMIC_GRASPING_SERVER_ROOT");
        if(env_root_folder_path == NULL) {
            output_string_ = "The environment variable $MIMIC_GRASPING_SERVER_ROOT is not defined";
            return;
        }

        root_folder_path_ = std::string(env_root_folder_path);

        if(!loadFirmwareInterfaceConfigFile(root_folder_path_ + config_folder_path_ + tool_firmware_file_ ) ||
           !startToolCommunication(output_string_) ||
           !setGripperType(ToolFirmwareInterface::GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER)) //TODO: get from config file
            return;

        // TODO: run and test localizations modules
        bool stop_ = false;
        int current_code;

        std::string current_msg = "";
        while(!stop_) {
            current_msg = received_msg_;
            convertMsgToCode(current_msg,current_code);

            output_string_ = current_msg;
            std::cout << output_string_ << std::endl;

            if(current_code == ToolFirmwareInterface::MSG_TYPE::STATE_SAVING)
            {
                output_string_ = "Save pose request received.";
                std::cout << output_string_ << std::endl;
                sendSuccessMsg();
                //TODO
            }
            else if(current_code == ToolFirmwareInterface::MSG_TYPE::STATE_CANCELLING){
                output_string_ = "Remove last save pose request received.";
                std::cout << output_string_ << std::endl;
                sendSuccessMsg();
                //TODO
            }
            spinner_sleep(1000);
        }


    }

}//end namespace