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

        enum EXPORT_EXTENSION{
            YAML,
            JSON,
        };

        enum GRIPPER_ID {        // Type of gripper used
            ROBOTIQ_2F_85,       // two adaptive fingers gripper from Robotiq with opening of 85mm
            ROBOTIQ_2F_140,      // two adaptive fingers gripper from Robotiq with opening of 140mm
            ROBOTIQ_3F,           // three adaptive fingers gripper from Robotiq
            SUCTION,
            FESTO_2F_HGPC_16_A,   //Pneumatic parallel 2F gripper from FESTO
            SCHMALZ_SINGLE_RECT_X_SUCTION,
        };

        enum SYNTHESIS_METHOD {
            MANUAL,
            SANN_MULTIFINGERED,
            SANN_SUCTION,
            MIMIC_GRASPING,
        };

        void start();
        bool load();
        bool init();
        bool spin();
        void stop();
        void clearDataset();
        bool saveDataset(std::string _path, int _type);

        std::string getOutputSTR();
        int getCurrentStateCode();

    protected:
        char const* env_root_folder_path;
        std::string root_folder_path_,
                    plugins_folder_path_="/plugins",
                    config_folder_path_ = "/configs",
                    scripts_folder_ = "/scripts",
                    tool_firmware_file_ = "/tool_firmware_config.json",
                    localization_file_ = "/localization_config.json",
                    current_msg_ = "";

        int current_code_;
        bool stop_ = false;
        bool requestToolLocalization();
        bool requestObjectLocalization();
        bool exportJSONDataset(std::string _path);
        bool exportYAMLDataset(std::string _path);

    private:
        std::string output_string_;
        Pose current_obj_pose_,
             current_tool_pose_;
        std::vector<Pose> obj_pose_arr_,
                          tool_pose_arr_;
    }; // end class

} // end namespace


#endif //MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
