
#ifndef MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
#define MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H

#include <iostream>
#include <thread>
#include <boost/thread.hpp>
#include <sys/types.h>
#include <sys/stat.h>

#include <simple_serial/simple_serial.h>
#include "tool_firmware_interface.h"
#include "localization_interface.h"
#include "dataset_manipulator.h"

#define MSG_PREFIX "<MimicGraspingAPI> "

#define JSON_GENERAL_TAG "general"
#define JSON_PROFILE_TAG "profile"

#ifndef NDEBUG
#define DEBUG_MSG(str) do { std::cout << "\033[;33m" << MSG_PREFIX << str << "\033[0m"<< std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

namespace mimic_grasping{

    class MimicGraspingServer: public ToolFirmwareInterface, public LocalizationInterface, public DatasetManipulator{

    public:
        MimicGraspingServer();
        MimicGraspingServer(std::string _profile);
        ~MimicGraspingServer();

        void setProfile(std::string _profile);
        std::string getProfile();

        bool start();
        bool stop();

        bool setup();
        bool load();
        bool init();

        bool spin();
        void request_stop();
        bool closeInterfaces();
        void clearDataset();

        bool saveGeneralConfigFile(std::string _file);
        bool loadGeneralConfigFile(std::string _file);

        bool loadGeneralProfileConfigFile(std::string _file);
        bool saveGeneralProfileConfigFile(std::string _file);


        std::vector<Pose> getDataset(int _dataset_type);
        Pose getDataset(int _dataset_type, int _index);
        std::string getOutputExportPath();
        std::string getPluginsFolderPath();
        std::string getProfileFolderPath();
        std::string getConfigFolderPath();

        std::string getScriptFolderPath();
        bool getProfileListAt(int _index, std::string &_outputStr);

        bool generateProfileDirectoryTemplate();


        bool exportDatasets();
        bool isDatasetEmpty();
        int datasetSize();

        std::string getOutputSTR();
        std::string getErrorStr();

        int getCurrentStateCode();


    protected:
        char const* env_root_folder_path;
        std::string root_folder_path_,
                    plugins_folder_dir_="/plugins",
                    config_folder_dir_ = "/configs",
                    scripts_folder_dir_ = "/scripts",
                    tool_firmware_file_ = "/tool_firmware_config.json",
                    localization_file_ = "/localization_config.json",
                    current_msg_ = "",
                    profile_,
                    general_ = "/general.json",
                    general_profile_config_file_name_ = "/general_profile_config.json";

        Json::Value general_config_data_,
                    general_profile_config_data_;


        int current_code_, last_current_code_;
        bool stop_ = false;
        bool requestToolLocalization();
        bool requestObjectLocalization();



    private:
        std::string output_string_, error_string_, output_export_path_, plugins_folder_path_, script_folder_path_, profile_folder_path_, config_folder_path_, general_config_folder_path_, general_profile_config_folder_path_;
        Pose current_obj_pose_,
             current_tool_pose_;
        std::vector<Pose> obj_pose_arr_,
                          tool_pose_arr_,
                          tool_pose_wrt_obj_frame_arr_;
        std::vector<std::string> profile_list_;

        bool initialized_firmware_communication_ = false, initialized_obj_localization_ = false, initialized_tool_localization_ = false, plugin_list_is_loaded_ = false, running_ = false;
        bool buildProfileList();


    }; // end class

} // end namespace


#endif //MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
