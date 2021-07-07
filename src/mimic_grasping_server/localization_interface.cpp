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

    int index = (getIndexOfPlugin(localization_data_.object_localization_plugin_name));
    if(index < 0){
        output_string_ = "Cannot init the object localization. There is no plugin \""+ localization_data_.object_localization_plugin_name +"\" into the loaded plugin list.";
        return false;
    }

    //auto p = CreateInstanceAs<LocalizationBase>(GetPluginFactoryInfo(index)->Name(),
    //                                            GetPluginFactoryInfo(index)->GetClassName(0));

    obj_localization_obj_.reset();
    obj_localization_obj_ = CreateInstanceAs<LocalizationBase>(index,0);

    if(obj_localization_obj_ == nullptr){
        output_string_ = "Cannot init the object localization. Cannot instantiate the object localization.";
        return false;
    }
    if(!obj_localization_obj_->setAppExecPath(_path+"/"+localization_data_.object_command)
        || !obj_localization_obj_->runApp()){

        output_string_ = obj_localization_obj_->getOutputString();

        return false;

    }
    obj_localization_obj_->spin(1000);
    output_string_ = obj_localization_obj_->getOutputString();

    if(obj_localization_obj_->getStatus() == LocalizationBase::FEEDBACK::ERROR){
        return false;
    }

    return true;
}

bool LocalizationInterface::requestObjPose(Pose& _pose){
    return obj_localization_obj_->requestData(_pose);
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

bool LocalizationInterface::localization_spinner_sleep(int usec){
    obj_localization_obj_->spin(usec);
    if(obj_localization_obj_->getStatus() == LocalizationBase::ERROR){
        output_string_ = obj_localization_obj_->getOutputString();
        return false;
    }
    return true;
}