//
// Created by joaopedro on 07/07/21.
//

#ifndef MIMIC_GRASPING_SERVER_OBJ_LOCALIZATION_ROS_H
#define MIMIC_GRASPING_SERVER_OBJ_LOCALIZATION_ROS_H

#include <mimic_grasping_api/localization_base.h>

#include <fcntl.h> // fcntl function
#include <unistd.h> //read function
#include <boost/thread.hpp>
#include <fstream>

class ObjLocalizationROS : public LocalizationBase {
public:
    ObjLocalizationROS();
    ~ObjLocalizationROS();

    bool setAppConfigPath(std::string _path);
    bool setAppExec(std::string _file_with_path_or_command);
    bool setAppTermination(std::string _file_with_path_or_command);
    bool setTargetName(std::string _name);
    bool loadAppConfiguration() {return true;};
    bool saveAppConfiguration(std:: string _path)  {return true;};
    bool runApp();
    bool stopApp();
    bool requestData(Pose& _result);
    int getStatus();
    std::string getOutputString();
    void spin(int _usec);

private:
    std::shared_ptr<boost::thread> obj_localization_thread_reader_;
    bool first_obj_localization_communication_ = true;
    FILE *pipe_to_obj_localization_ = NULL;
    void execCallback(int _descriptor);
    void exec(int _descriptor);
    bool err_flag_pipe_corrupted_ = false;
    std::string current_pipe_output_str_; // TODO: it seems to be useless in current code version

};

/** ################## Factory Function - Plugin EntryPoint  ##################  **/
/** This part should be added in all plugins, therefore the plugin management can identified it. **/

/** Define the Plugin name **/
std::string plugin_name = "ObjLocalizationROSPlugin";

PLUGIN_EXPORT_C
auto GetPluginFactory() -> IPluginFactory * {

static PluginFactory pinfo = [] {
    /** Properly set the plugin name and version **/
    auto p = PluginFactory(plugin_name.c_str(), "07_06_2021");
    /** Register all classes defined inside the plugin **/
    p.registerClass<ObjLocalizationROS>("ObjLocalizationROS");
    return p;
}();
return &pinfo;
}

struct _DLLInit {
    _DLLInit() {
        std::cout << " [TRACE] Shared library " << plugin_name << " loaded OK.\n";
    }

    ~_DLLInit() {
        std::cout << " [TRACE] Shared library " << plugin_name << " unloaded OK.\n";
    }
} dll_init;

#endif //MIMIC_GRASPING_SERVER_OBJ_LOCALIZATION_ROS_H
