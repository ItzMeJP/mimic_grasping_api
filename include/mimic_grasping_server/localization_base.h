/**\file
 * \brief
 * Localization interface class to plugin manager
 * @version 02.07.2021
 * @author João Pedro Carvalho de Souza
 */

#ifndef MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H
#define MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H

#include "plugin_system_management/factory.h"
#include <mimic_grasping_server/pose.h>
#include <iostream>

class LocalizationBase {
public:
    LocalizationBase() {};
    ~LocalizationBase() {};

    enum FEEDBACK{
        PROCESSING = 100,
        RUNNING,
        FINISHED,
        ERROR,
        ABORTED
    };

    virtual bool setAppConfigPath(std::string _path) = 0;
    virtual bool setAppExecPath(std::string _path) = 0;
    virtual bool runApp() = 0;
    virtual bool requestData(Pose &_result) = 0;
    virtual int getStatus() = 0;
    virtual std::string getOutputString() = 0;
    virtual void spin(int _usec) = 0;

protected:
    std::string plugin_config_path_ = "",
                plugin_exec_path_ = "",
                output_string_;

    int status_;

};

#endif //MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H
