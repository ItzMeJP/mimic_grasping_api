#ifndef MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
#define MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H

#include <iostream>
#include <thread>

#include <simple_serial.h>
#include <boost/thread.hpp>
#include "tool_firmware_interface.h"


namespace mimic_grasping{

    class MimicGraspingServer: public ToolFirmwareInterface{

    public:
        MimicGraspingServer();
        ~MimicGraspingServer();

        void start();
        bool load();
        bool init();
        bool spin();
        void stop();

        int getCurrentStateCode();

    protected:
        char const* env_root_folder_path;
        std::string root_folder_path_,
                    config_folder_path_ = "/configs",
                    tool_firmware_file_ = "/tool_firmware_config.json",
                    current_msg_ = "";
        int current_code_;
        bool stop_ = false;
        bool requestToolLocalization();
        bool requestObjectLocalization();


    }; // end class

} // end namespace


#endif //MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
