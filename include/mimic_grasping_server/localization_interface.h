//
// Created by joaopedro on 05/07/21.
//

#ifndef MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H
#define MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H

#include "plugin_system_management/plugin_system_management.h"

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>

class LocalizationInterface : public PluginSystemManagement {
public:

    LocalizationInterface();
    ~LocalizationInterface();

    bool saveLocalizationConfigFile(std::string _file);
    bool loadLocalizationConfigFile(std::string _file);

    struct LocalizationData{
        std:: string plugin_folder_path,
        tool_localization_plugin_name,
        object_localization_plugin_name,
        tool_command,
        object_command;

    } localization_data_;

protected:
    Json::Value localization_interface_config_data_;
    std::string getLocalizationOutputSTR();

private:
    std::string output_string_;

};


#endif //MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H
