//
// Created by joaopedro on 05/07/21.
//

#ifndef MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H
#define MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H

#include "plugin_system_management/plugin_system_management.h"

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <fcntl.h> // fcntl function
#include <unistd.h> //read function
#include <boost/thread.hpp>


#ifndef JSON_TAGS_DEFINE_H
#define JSON_TAGS_DEFINE_H

#define JSON_PLUGIN_MANAGER_TAG "plugin_manager"
#define JSON_TOOL_LOC_TAG "tool_localization"
#define JSON_OBJ_LOC_TAG "object_localization"
#define JSON_PL_NAME_TAG "plugin_name"
#define JSON_EX_CMD_TAG "exec_command"
#define JSON_PL_FOLDER_PTH "folder_path"

#endif // JSON_TAGS_DEFINE_H


class LocalizationInterface : public PluginSystemManagement {
public:

    LocalizationInterface();

    ~LocalizationInterface();

    struct LocalizationData {
        std::string plugin_folder_path,
                tool_localization_plugin_name,
                object_localization_plugin_name,
                tool_command,
                object_command;

    } localization_data_;

    enum LOCALIZATION_TYPE{
        TOOL,
        OBJ,
    };

    bool saveLocalizationConfigFile(std::string _file);

    bool loadLocalizationConfigFile(std::string _file);

    bool initToolLocalization(std::string _path);

    bool initObjLocalization(std::string _path);

    void obj_localization_spinner_sleep(int _usec);

    std::string getLocalizationInterfaceOutputSTR();


protected:
    Json::Value localization_interface_config_data_;

    std::string getLocalizationOutputSTR();

    std::string execIt(int _file_descriptor, int _type);
    void execCallback(int _file_descriptor,int _type);

private:
    std::string output_string_;
    std::shared_ptr<boost::thread> obj_localization_thread_reader_,
                                   tool_localization_thread_reader_;

    bool first_obj_localization_communication_ = true;
    FILE *pipe_to_obj_localization_ = NULL,
         *pipe_to_tool_localization_= NULL;




};


#endif //MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H
