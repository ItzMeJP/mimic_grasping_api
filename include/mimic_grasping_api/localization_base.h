/**\file
 * \brief
 * Localization interface class to plugin manager
 * @version 02.07.2021
 * @author João Pedro Carvalho de Souza
 */

#ifndef MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H
#define MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H

#include "plugin_system_management/factory.h"
#include <transform_manipulation/pose.h>
#include <iostream>
#include <unistd.h>

class LocalizationBase {
public:
    LocalizationBase() {};
    ~LocalizationBase() {};

    enum FEEDBACK{
        INITIALIZING = 100,
        PROCESSING,
        RUNNING,
        FINISHED,
        ERROR,
        ABORTED
    };

    virtual bool setAppConfigPath(std::string _file_with_path) = 0;
    virtual bool setAppExec(std::string _file_with_path_or_command) = 0;
    virtual bool setAppTermination(std::string _file_with_path_or_command) = 0;
    virtual bool loadAppConfiguration() = 0;
    virtual bool runApp() = 0;
    virtual bool stopApp() = 0;
    virtual bool setTargetName(std::string _name) = 0;
    virtual bool requestData(Pose &_result) = 0;
    virtual int getStatus() = 0;
    virtual std::string getOutputString() = 0;
    virtual void spin(int _usec) = 0;

protected:
    std::string plugin_config_path_ = "",
                plugin_exec_path_ = "",
                plugin_terminator_path_ = "",
                output_string_,
                target_name_;

    Pose current_pose_;

    int status_;

};

#endif //MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H
