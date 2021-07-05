#ifndef MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
#define MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H

#include <iostream>
#include <thread>

#include <simple_serial/simple_serial.h>
#include <boost/thread.hpp>
#include "mimic_grasping_server/tool_firmware_interface.h"
#include "mimic_grasping_server/localization_interface.h"

namespace mimic_grasping{

    class MimicGraspingServer: public ToolFirmwareInterface, LocalizationInterface{

    public:
        MimicGraspingServer();
        ~MimicGraspingServer();

        void start();
        bool load();
        bool init();
        bool spin();
        void stop();

        std::string getOutputSTR();
        int getCurrentStateCode();

    protected:
        char const* env_root_folder_path;
        std::string root_folder_path_,
                    plugins_folder_path_="/plugins",
                    config_folder_path_ = "/configs",
                    tool_firmware_file_ = "/tool_firmware_config.json",
                    localization_file_ = "/localization_config.json",
                    current_msg_ = "";

        int current_code_;
        bool stop_ = false;
        bool requestToolLocalization();
        bool requestObjectLocalization();

    private:
        std::string output_string_;
    }; // end class

} // end namespace


#endif //MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
