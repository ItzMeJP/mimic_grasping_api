
#ifndef MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
#define MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H

#include <iostream>
#include <thread>
#include <boost/thread.hpp>

#include <simple_serial/simple_serial.h>
#include "tool_firmware_interface.h"
#include "localization_interface.h"
#include "dataset_manipulator.h"

#define MSG_PREFIX "<MimicGraspingAPI> "

#ifndef NDEBUG
#define DEBUG_MSG(str) do { std::cout << "\033[;33m" << MSG_PREFIX << str << "\033[0m"<< std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

namespace mimic_grasping{

    class MimicGraspingServer: public ToolFirmwareInterface, LocalizationInterface, DatasetManipulator{

    public:
        MimicGraspingServer();
        ~MimicGraspingServer();

        bool start();
        bool load();
        bool init();
        bool spin();
        void stop();
        bool closeInterfaces();
        void clearDataset();
        bool exportDatasets();

        std::string getOutputSTR();
        int getCurrentStateCode();

    protected:
        char const* env_root_folder_path;
        std::string root_folder_path_,
                    plugins_folder_dir_="/plugins",
                    config_folder_dir_ = "/configs",
                    scripts_folder_dir_ = "/scripts",
                    tool_firmware_file_ = "/tool_firmware_config.json",
                    localization_file_ = "/localization_config.json",
                    current_msg_ = "";

        int current_code_;
        bool stop_ = false;
        bool requestToolLocalization();
        bool requestObjectLocalization();



    private:
        std::string output_string_;
        Pose current_obj_pose_,
             current_tool_pose_;
        std::vector<Pose> obj_pose_arr_,
                          tool_pose_arr_;



    }; // end class

} // end namespace


#endif //MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
