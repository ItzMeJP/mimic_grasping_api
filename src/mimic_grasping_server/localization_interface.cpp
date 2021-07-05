//
// Created by joaopedro on 05/07/21.
//

#define JSON_PLUGIN_MANAGER_TAG "plugin_manager"
#define JSON_TOOL_LOC_TAG "tool_localization"
#define JSON_OBJ_LOC_TAG "object_localization"
#define JSON_PL_NAME_TAG "plugin_name"
#define JSON_EX_CMD_TAG "exec_command"
#define JSON_PL_FOLDER_PTH "folder_path"

#include "mimic_grasping_server/localization_interface.h"

LocalizationInterface::LocalizationInterface(){};

LocalizationInterface::~LocalizationInterface(){};

bool LocalizationInterface::saveLocalizationConfigFile(std::string _file){

    localization_interface_config_data_[JSON_PLUGIN_MANAGER_TAG][JSON_PL_FOLDER_PTH] = localization_data_.plugin_folder_path;
    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG] = localization_data_.tool_localization_plugin_name;
    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG] = localization_data_.tool_command;
    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG] = localization_data_.object_localization_plugin_name;
    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG] = localization_data_.object_command;

    std::ofstream outfile (_file);
    outfile << localization_interface_config_data_ << std::endl;
    outfile.close();

    return true;
}

bool LocalizationInterface::loadLocalizationConfigFile(std::string _file) {

    std::ifstream config_file(_file, std::ifstream::binary);
    if(config_file){
        try {
            config_file >> localization_interface_config_data_;
        } catch (const std::exception& e) {
            //std::cerr << e.what() << std::endl;
            output_string_ = e.what();
            return false;
        }
    }
    else{
        output_string_ = "Localization configuration file not found.";
        return false;
    }

    localization_data_.plugin_folder_path = localization_interface_config_data_[JSON_PLUGIN_MANAGER_TAG][JSON_PL_FOLDER_PTH].asString();
    localization_data_.tool_localization_plugin_name = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG].asString();
    localization_data_.tool_command = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG].asString();
    localization_data_.object_localization_plugin_name = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG].asString();
    localization_data_.object_command = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG].asString();

    return true;
}

std::string LocalizationInterface::getLocalizationOutputSTR(){
    return output_string_;
}