//
// Created by joaopedro on 09/06/21.
//

#include "mimic_grasping_server/tool_firmware_interface.h"

namespace mimic_grasping {

    ToolFirmwareInterface::ToolFirmwareInterface() {
        serial_ = std::make_shared<SimpleSerial>();
    }

    ToolFirmwareInterface::~ToolFirmwareInterface() {
        stopToolCommunication();
    }


    bool ToolFirmwareInterface::loadFirmwareInterfaceConfigFile(std::string _file) {

        std::ifstream config_file(_file, std::ifstream::binary);
        if (config_file) {
            try {
                config_file >> tool_firmware_interface_config_data_;
            } catch (const std::exception &e) {
                //std::cerr << e.what() << std::endl;
                output_string_ = e.what();
                return false;
            }
        } else {
            output_string_ = "Firmware configuration file not found.";
            return false;
        }

        current_gripper_type_ = tool_firmware_interface_config_data_["tool"]["type"].asInt();
        serial_->setBaudRate(tool_firmware_interface_config_data_["serial"]["baud_rate"].asDouble());
        serial_->setPort(tool_firmware_interface_config_data_["serial"]["port"].asString());

        return true;
    }

    bool ToolFirmwareInterface::initToolFirmware() {

        if (!startToolCommunication(output_string_) ||
            !setGripperType(current_gripper_type_))
            return false;

        return true;
    }

    bool ToolFirmwareInterface::setSerialConfig(std::string _port, unsigned int _baud_rate, std::string &_output_str) {

        serial_->setPort(_port);
        serial_->setBaudRate(_baud_rate);
        //serial_thread_reader_ = std::make_shared<boost::thread>(boost::bind(&MimicGraspingServer::readCommandCallback, this));

        //return startToolCommunication(_output_str);
        return true;
    }

    int ToolFirmwareInterface::getBaudRate() {
        return serial_->getBaudRate();
    }

    std::string ToolFirmwareInterface::getPort() {
        return serial_->getPort();
    }

    int ToolFirmwareInterface::getGripperType() {
        return current_gripper_type_;
    }

    bool ToolFirmwareInterface::startToolCommunication(std::string &_output_str) {

        err_flag_communication_corrupted_ = false;

        stopToolCommunication();

        if (!serial_->start(_output_str)) {
            output_string_ = "Error in starting serial communication. " + _output_str;
            return false;
        }
        received_msg_ = "";
        //serial_thread_reader_.reset(new std::thread(&MimicGraspingServer::readCommandCallback, this));
        serial_thread_reader_.reset(new boost::thread(boost::bind(&ToolFirmwareInterface::readCommandCallback, this)));

        if (first_tool_communication_)
            serial_thread_reader_->timed_join(
                    boost::chrono::milliseconds(2500)); // Waiting to arduino boot and start communication

        first_tool_communication_ = false;
        return true;
    }

    bool ToolFirmwareInterface::stopToolCommunication() {
        if (!first_tool_communication_) {
            writeSerialCommand(MSG_TYPE::RESET); // put the tool to initial state
            serial_thread_reader_->interrupt();
            serial_thread_reader_->join();

            std::string _output_str;
            if (!serial_->stop(_output_str)) {
                output_string_ = "Error in stopping serial communication. " + _output_str;
                return false;
            }
        }
        else {
            output_string_ = "Cannot stop serial communication since the process is not running.";
            return false;
        }

        first_tool_communication_ = true;
        return true;
    }

    bool ToolFirmwareInterface::isFirmwareCommunicationInitialized() {
        return !first_tool_communication_;
    }


    void ToolFirmwareInterface::writeSerialCommand(std::string _s) {
        serial_->writeString(_s);
    }

    void ToolFirmwareInterface::writeSerialCommand(int _s) {
        serial_->writeString(std::to_string(_s));
    }

    bool ToolFirmwareInterface::readCommand(std::string &_msg) {
        std::string error_msg = "";
        if (!serial_->readLine(_msg, error_msg)) {
            output_string_ = "Connection with tool firmware lost. " + error_msg;
            err_flag_communication_corrupted_ = true;
            return false;
        }

        return true;
    }

    void ToolFirmwareInterface::readCommandCallback() {
        for (;;) {
            try {
                this->readCommand(received_msg_);
                boost::this_thread::interruption_point();
                //boost::this_thread::sleep(boost::posix_time::milliseconds(500)); //interruption with sleep
            }
            catch (boost::thread_interrupted &) {
                std::cout << "Tool firmware thread is stopped" << std::endl;
                return;
            }
        }
    }

    bool ToolFirmwareInterface::convertMsgToCode(std::string _msg, int &_code) {
        std::string delimiter = "#", token;
        if (!(_msg.find(delimiter) != std::string::npos)) {
            output_string_ = "The message does not represent a code.";
            return false;
        }
        token = _msg.substr(_msg.find(delimiter) + delimiter.length(), std::string::npos);
        output_string_ = "Identified code: " + token;
        //std::cout << output_string_ << std::endl;
        _code = std::stoi(token);
        return true;

    }

    bool ToolFirmwareInterface::firmware_spinner_sleep(int _usec) {
        serial_thread_reader_->timed_join(boost::chrono::milliseconds(_usec));
        if (err_flag_communication_corrupted_) {
            output_string_ = "Corrupted serial communication.";
            serial_thread_reader_->interrupt();
            serial_thread_reader_->join();
            return false;
        }
        return true;
    }

    bool ToolFirmwareInterface::setGripperType(int _gripper) {
        if (!resetFirmware())
            return false;

        writeSerialCommand(_gripper);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(2500));
        firmware_spinner_sleep(2500);
        std::string msg = received_msg_;

        if (!(msg.find("#" + std::to_string(MSG_TYPE::STATE_RUNNING)) != std::string::npos)) {
            output_string_ = "Gripper not initialized.";
            return false;
        }
        output_string_ = "Gripper initialized.";
        current_gripper_type_ = _gripper;
        //std::cout << output_string_ << std::endl;
        return true;
    }

    bool ToolFirmwareInterface::sendCustomMSG(std::string _in) {
        writeSerialCommand(_in);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(1000));
        firmware_spinner_sleep(2000);
        std::string msg = received_msg_;
        output_string_ = "Custom message sent. Feedback msg: " + msg;
        return true;
    }

    bool ToolFirmwareInterface::resetFirmware() {
        writeSerialCommand(MSG_TYPE::RESET);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(1000));
        firmware_spinner_sleep(2000);
        std::string msg = received_msg_;
        if (!(msg.find("#" + std::to_string(MSG_TYPE::STATE_INIT)) != std::string::npos)) {
            output_string_ = "Firmware is not able to be reset";
            return false;
        }
        output_string_ = "Firmware reset";
        //std::cout << output_string_ << std::endl;
        return true;
    }

    bool ToolFirmwareInterface::sendErrorMsg() {
        writeSerialCommand(MSG_TYPE::ERROR);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(250));
        firmware_spinner_sleep(250);
        std::string msg = received_msg_;
        if (!(msg.find("#" + std::to_string(MSG_TYPE::STATE_ERROR)) != std::string::npos)) {
            output_string_ = "Error msg not recognized.";
            //std::cout << output_string_ << std::endl;
            return false;
        }
        output_string_ = "Error msg recognized.";
        //std::cout << output_string_ << std::endl;
        return true;
    }

    bool ToolFirmwareInterface::sendSuccessMsg() {
        writeSerialCommand(MSG_TYPE::SUCCESS);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(250));
        firmware_spinner_sleep(250);
        std::string msg = received_msg_;
        if (!(msg.find("#" + std::to_string(MSG_TYPE::STATE_SUCCESS)) != std::string::npos) &&
            !(msg.find("Last save canceled") != std::string::npos)) {
            output_string_ = "Success msg not recognized.";
            //std::cout << output_string_ << std::endl;
            return false;
        }
        output_string_ = "Success msg recognized.";
        //std::cout << output_string_ << std::endl;
        return true;
    }

    /*
    bool ToolFirmwareInterface::run()
    {
        if(!readConfigFile() ||
           !startToolCommunication(output_string_) ||
           !setGripperType(ToolFirmwareInterface::GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER))
            return false;
        return true;
    }
    */

    bool ToolFirmwareInterface::saveFirmwareInterfaceConfigFile(std::string _file) {

        tool_firmware_interface_config_data_["serial"]["port"] = serial_->getPort();
        tool_firmware_interface_config_data_["serial"]["baud_rate"] = serial_->getBaudRate();
        tool_firmware_interface_config_data_["tool"]["type"] = current_gripper_type_;

        std::ofstream outfile(_file);
        outfile << tool_firmware_interface_config_data_ << std::endl;
        outfile.close();

        return true;
    }

    std::string ToolFirmwareInterface::getCurrentMsg() {
        return received_msg_;
    }

    std::string ToolFirmwareInterface::getToolFirmwareOutputSTR() {
        return output_string_;
    }

}//end namespace