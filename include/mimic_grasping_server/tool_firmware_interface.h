//
// Created by joaopedro on 09/06/21.
//
#ifndef MIMIC_GRASPING_SERVER_TOOL_FIRMWARE_INTERFACE_H
#define MIMIC_GRASPING_SERVER_TOOL_FIRMWARE_INTERFACE_H

#include <iostream>
#include <thread>

#include <simple_serial.h>
#include <boost/thread.hpp>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <stdlib.h>

namespace mimic_grasping{

    class ToolFirmwareInterface{

    public:
        ToolFirmwareInterface();
        ~ToolFirmwareInterface();

        bool setSerialConfig(std::string _port, unsigned int _baud_rate, std::string &_output_str);
        void writeSerialCommand(std::string _command);
        void writeSerialCommand(int _s);
        std::string readCommand();
        bool startToolCommunication(std::string &_output_str);
        std::string getOutputString();
        bool setGripperType(int _gripper);
        bool resetFirmware();
        std::string getCurrentMsg();
        bool sendErrorMsg();
        bool sendSuccessMsg();
        bool saveFirmwareInterfaceConfigFile(std::string _file);
        bool loadFirmwareInterfaceConfigFile(std::string _file);
        bool convertMsgToCode(std::string _msg, int &_code);
        void spinner_sleep(int _usec);
        //bool run();

        enum MSG_TYPE
        {
            CONNECTION_STABILISHED_TWO_ALTERNATE_RELAYS = 100,
            CONNECTION_STABILISHED_ONE_RELAY,
            ACK,
            ERROR,
            SUCCESS,
            RESET,
            STATE_INIT = 500,
            STATE_RUNNING = 501,
            STATE_ACTIVE_GRIPPER = 502,
            STATE_SAVING = 503,
            STATE_ERROR = 504,
            STATE_CANCELLING = 505,
            STATE_SUCCESS = 506
        };

        enum GRIPPER_TYPE{
            PARALLEL_PNEUMATIC_TWO_FINGER = 100,
            SINGLE_SUCTION_CUP = 101
        };

    protected:

        std::shared_ptr<SimpleSerial> serial_;
        void readCommandCallback();
        //std::shared_ptr<std::thread> serial_thread_reader_;
        std::shared_ptr<boost::thread> serial_thread_reader_;

        std::string output_string_,
                received_msg_,
                firmware_state_delimiter_ = " >> ";

        bool first_tool_communication = true;

        int current_gripper_type_;
        Json::Value tool_firmware_interface_config_data_;



    }; // end class

} // end namespace

#endif //MIMIC_GRASPING_SERVER_TOOL_FIRMWARE_INTERFACE_H
