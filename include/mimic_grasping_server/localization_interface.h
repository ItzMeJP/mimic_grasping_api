//
// Created by joaopedro on 05/07/21.
//

#ifndef MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H
#define MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H

#include "plugin_system_management/plugin_system_management.h"
#include <mimic_grasping_server/localization_base.h>

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
#define JSON_EX_CMD_TAG "executor_command"
#define JSON_TERM_CMD_TAG "terminator_command"
#define JSON_PL_FOLDER_PTH "folder_path"

#endif // JSON_TAGS_DEFINE_H


class LocalizationInterface : public PluginSystemManagement {
public:

    LocalizationInterface();

    ~LocalizationInterface();

    struct LocalizationData {
        std::string plugin_name,
                executor,
                terminator;
    } obj_localization_data_,
      tool_localization_data_;

    enum LOCALIZATION_TYPE{
        TOOL,
        OBJ,
    };

    bool saveLocalizationConfigFile(std::string _file);

    bool loadLocalizationConfigFile(std::string _file);

    bool setScriptsFolderPath(std::string _path);

    bool initToolLocalization();

    bool initObjLocalization();

    bool stopObjLocalization();

    bool stopToolLocalization();

    bool localization_spinner_sleep(int usec);

    std::string getLocalizationInterfaceOutputSTR();

    bool requestObjPose(Pose &_pose);


protected:
    Json::Value localization_interface_config_data_;

    std::string getLocalizationOutputSTR();

private:
    std::string output_string_,
            scripts_folder_path_;
    std::shared_ptr<LocalizationBase> obj_localization_obj_,
                    tool_localization_obj_;
    bool isScript(std::string _s);
    bool initLocalization(std::shared_ptr<LocalizationBase>& _loc_instance, LocalizationData _data);

};


#endif //MIMIC_GRASPING_SERVER_PLUGINS_INTERFACE_H
