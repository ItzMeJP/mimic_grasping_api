//
// Created by joaopedro on 05/07/21.
//

#include "mimic_grasping_server/localization_interface.h"


LocalizationInterface::LocalizationInterface() {};

LocalizationInterface::~LocalizationInterface() {};

bool LocalizationInterface::saveLocalizationConfigFile(std::string _file) {

    //localization_interface_config_data_[JSON_PLUGIN_MANAGER_TAG][JSON_PL_FOLDER_PTH] = localization_data_.plugin_folder_path;
    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG] = localization_data_.tool_localization_plugin_name;
    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG] = localization_data_.tool_command;
    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG] = localization_data_.object_localization_plugin_name;
    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG] = localization_data_.object_command;

    std::ofstream outfile(_file);
    outfile << localization_interface_config_data_ << std::endl;
    outfile.close();

    return true;
}

bool LocalizationInterface::loadLocalizationConfigFile(std::string _file) {

    std::ifstream config_file(_file, std::ifstream::binary);
    if (config_file) {
        try {
            config_file >> localization_interface_config_data_;
        } catch (const std::exception &e) {
            //std::cerr << e.what() << std::endl;
            output_string_ = e.what();
            return false;
        }
    } else {
        output_string_ = "Localization configuration file not found.";
        return false;
    }

    //localization_data_.plugin_folder_path = localization_interface_config_data_[JSON_PLUGIN_MANAGER_TAG][JSON_PL_FOLDER_PTH].asString();
    localization_data_.tool_localization_plugin_name = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG].asString();
    localization_data_.tool_command = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG].asString();
    localization_data_.object_localization_plugin_name = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG].asString();
    localization_data_.object_command = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG].asString();

    return true;
}

std::string LocalizationInterface::getLocalizationOutputSTR() {
    return output_string_;
}

bool LocalizationInterface::initToolLocalization(std::string _path) {

    return true;
}

bool LocalizationInterface::initObjLocalization(std::string _path) {

    std::string cmd_name = "/obj_localization_ros.sh" ,
                cmd = _path + cmd_name;

    if(!first_obj_localization_communication_){
        obj_localization_thread_reader_->interrupt();
        obj_localization_thread_reader_->join();
        pclose(pipe_to_obj_localization_);
    }

    pipe_to_obj_localization_ = popen(cmd.c_str(), "r");

    int descriptor = fileno(pipe_to_obj_localization_);
    fcntl(descriptor, F_SETFL, O_NONBLOCK);

    obj_localization_thread_reader_.reset(new boost::thread (boost::bind(&LocalizationInterface::execCallback,this,descriptor,LOCALIZATION_TYPE::OBJ)));

    //if(first_obj_localization_communication_)
    //    obj_localization_thread_reader_->timed_join(boost::chrono::milliseconds(2500)); // Waiting to arduino boot and start communication


    //std::cout << execIt(cmd.c_str(), 8.0) << std::endl;
    first_obj_localization_communication_ = false;

    return true;
}

/*
std::string LocalizationInterface::execIt(const char *cmd, float _startup_delay_seconds) {

    float timeout = 0, deadline = _startup_delay_seconds;
    char buffer[128];
    std::string result;
    FILE *pipe = popen(cmd, "r");
    int descriptor = fileno(pipe);
    fcntl(descriptor, F_SETFL, O_NONBLOCK);

    const clock_t begin_time = clock();

    while (timeout < deadline) {
        timeout = float(clock() - begin_time) / CLOCKS_PER_SEC;
        ssize_t r = read(descriptor, buffer, 128);

        if (r == -1 && errno == EAGAIN) {}
        else if (r > 0) {
            result += buffer;
        } else
            std::cout << "Pipe closed. " << std::endl;
    }

    return result;
}
*/

std::string LocalizationInterface::getLocalizationInterfaceOutputSTR(){
    return output_string_;
}


std::string LocalizationInterface::execIt(int _file_descriptor,  int _type) {

    char buffer[128];
    std::string result;

    ssize_t r = read(_file_descriptor, buffer, 128);

    if (r == -1 && errno == EAGAIN) {}
    else if (r > 0) {
        result += buffer;
    } else{
        if(_type == LOCALIZATION_TYPE::OBJ){
            std::cout << "Corrupted pipe to object localization interface. " << std::endl;
            output_string_ = "Corrupted pipe to object localization interface. ";
        }
        else{
            std::cout << "Corrupted pipe to tool localization interface. " << std::endl;
            output_string_ = "Corrupted pipe to tool localization interface. ";
        }

    }

    return result;
}

void LocalizationInterface::execCallback(int _file_descriptor, int _type){
    for(;;) {
        try {
            output_string_ = this->execIt(_file_descriptor,_type);
            boost::this_thread::interruption_point();
            //boost::this_thread::sleep(boost::posix_time::milliseconds(500)); //interruption with sleep
        }
        catch(boost::thread_interrupted&)
        {
            std::cout << "Localization thread is stopped" << std::endl;
            return;
        }
    }
}

void LocalizationInterface::obj_localization_spinner_sleep(int _usec){
    obj_localization_thread_reader_->timed_join(boost::chrono::milliseconds(_usec));

}