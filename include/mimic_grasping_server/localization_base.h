/**\file
 * \brief
 * Localization interface class to plugin manager
 * @version 02.07.2021
 * @author João Pedro Carvalho de Souza
 */

#ifndef MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H
#define MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H

#include "plugin_system_management/factory.h"
#include <pose.h>
#include <iostream>

class LocalizationBase {
public:
    LocalizationBase() {};
    ~LocalizationBase() {};

    enum FEEDBACK{
        PROCESSING = 100,
        FINISHED,
        ERROR,
        ABORTED
    };

    virtual bool setAppConfigPath(std::string _path) = 0;
    virtual bool setAppExecPath(std::string _path) = 0;
    virtual bool runApp() = 0;
    virtual bool requestData(Pose &_result) = 0;
    virtual int getStatus() = 0;

};

#endif //MIMIC_GRASPING_SERVER_LOCALIZATION_BASE_H
