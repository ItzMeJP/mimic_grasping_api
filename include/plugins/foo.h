//
// Created by joaopedro on 02/07/21.
//

#ifndef MIMIC_GRASPING_SERVER_FOO_H
#define MIMIC_GRASPING_SERVER_FOO_H

#include <mimic_grasping_server/localization_base.h>

class Foo : public LocalizationBase {
public:
    Foo();
    ~Foo();

    bool setAppConfigPath(std::string _path);
    bool setAppExecPath(std::string _path);
    bool runApp();
    bool requestData(Pose& _result);
    int getStatus();

};

/** ################## Factory Function - Plugin EntryPoint  ##################  **/
/** This part should be added in all plugins, therefore the plugin management can identified it. **/

/** Define the Plugin name **/
std::string plugin_name = "FooPlugin";

PLUGIN_EXPORT_C
auto GetPluginFactory() -> IPluginFactory * {

static PluginFactory pinfo = [] {
    /** Properly set the plugin name and version **/
    auto p = PluginFactory(plugin_name.c_str(), "01_06_2021");
    /** Register all classes defined inside the plugin **/
    p.registerClass<Foo>("Foo");
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

#endif //FOO_PLUGIN_H


