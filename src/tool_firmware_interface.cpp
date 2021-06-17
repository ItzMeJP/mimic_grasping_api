//
// Created by joaopedro on 09/06/21.
//

#include "tool_firmware_interface.h"

namespace mimic_grasping {

    ToolFirmwareInterface::ToolFirmwareInterface(){
        serial_ = std::make_shared<SimpleSerial>();
    }

    ToolFirmwareInterface::~ToolFirmwareInterface(){

    }



    bool ToolFirmwareInterface::loadFirmwareInterfaceConfigFile(std::string _file) {

        std::ifstream config_file(_file, std::ifstream::binary);
        config_file >> tool_firmware_interface_config_data_;

        current_gripper_type_ = tool_firmware_interface_config_data_["tool"]["type"].asInt();
        serial_->setBaudRate(tool_firmware_interface_config_data_["serial"]["baud_rate"].asDouble());
        serial_->setPort(tool_firmware_interface_config_data_["serial"]["port"].asString());

        return true;
    }

    bool ToolFirmwareInterface::setSerialConfig(std::string _port, unsigned int _baud_rate, std::string &_output_str){

        serial_->setPort(_port);
        serial_->setBaudRate(_baud_rate);
        //serial_thread_reader_ = std::make_shared<boost::thread>(boost::bind(&MimicGraspingServer::readCommandCallback, this));

        return startToolCommunication(_output_str);

    }

    bool ToolFirmwareInterface::startToolCommunication(std::string &_output_str) {

        if(!first_tool_communication){
            writeSerialCommand(MSG_TYPE::RESET); // put the tool to initial state
            serial_thread_reader_->interrupt();
            serial_thread_reader_->join();
        }

        if(!serial_->start(_output_str)){
            return false;
        }
        received_msg_ = "";
        //serial_thread_reader_.reset(new std::thread(&MimicGraspingServer::readCommandCallback, this));
        serial_thread_reader_.reset(new boost::thread (boost::bind(&ToolFirmwareInterface::readCommandCallback, this)));

        if(first_tool_communication)
            serial_thread_reader_->timed_join(boost::chrono::milliseconds(2500)); // Waiting to arduino boot and start communication

        first_tool_communication = false;
        return true;
    }


    void ToolFirmwareInterface::writeSerialCommand(std::string _s){
        serial_->writeString(_s);
    }

    void ToolFirmwareInterface::writeSerialCommand(int _s){
        serial_->writeString(std::to_string(_s));
    }

    std::string ToolFirmwareInterface::readCommand(){
        return serial_->readLine();
    }

    void ToolFirmwareInterface::readCommandCallback(){
        for(;;) {
            try {
                received_msg_ = this->readCommand();
                boost::this_thread::interruption_point();
                //boost::this_thread::sleep(boost::posix_time::milliseconds(500)); //interruption with sleep
            }
            catch(boost::thread_interrupted&)
            {
                std::cout << "Thread is stopped" << std::endl;
                return;
            }
        }
    }

    bool ToolFirmwareInterface::convertMsgToCode(std::string _msg, int &_code)
    {
        std::string delimiter = "#", token;
        if(!(_msg.find(delimiter)!= std::string::npos)){
            output_string_ = "The message does not represent a code.";
            return false;
        }
        token = _msg.substr(_msg.find(delimiter)+delimiter.length(), std::string::npos);
        output_string_ = "Identified code: " + token;
        //std::cout << output_string_ << std::endl;
        _code = std::stoi(token);
        return true;

    }

    void ToolFirmwareInterface::spinner_sleep(int _usec){
        serial_thread_reader_->timed_join(boost::chrono::milliseconds(_usec));
    }

    bool ToolFirmwareInterface::setGripperType(int _gripper)
    {
        if(!resetFirmware())
            return false;

        writeSerialCommand(_gripper);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(2500));
        spinner_sleep(2500);
        std::string msg = received_msg_;

        if(!(msg.find("#"+std::to_string(MSG_TYPE::STATE_RUNNING)) != std::string::npos)){
            output_string_ = "Gripper not initialized";
            return false;
        }
        output_string_ = "Gripper initialized";
        current_gripper_type_ = _gripper;
        //std::cout << output_string_ << std::endl;
        return true;
    }

    bool ToolFirmwareInterface::resetFirmware(){
        writeSerialCommand(MSG_TYPE::RESET);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(1000));
        spinner_sleep(1000);
        std::string msg = received_msg_;
        if(!(msg.find("#"+std::to_string(MSG_TYPE::STATE_INIT))!= std::string::npos)){
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
        spinner_sleep(250);
        std::string msg = received_msg_;
        if(!(msg.find("#"+std::to_string(MSG_TYPE::STATE_ERROR))!= std::string::npos)){
            output_string_ = "Error msg not recognized";
            //std::cout << output_string_ << std::endl;
            return false;
        }
        output_string_ = "Error msg recognized";
        //std::cout << output_string_ << std::endl;
        return true;
    }

    bool ToolFirmwareInterface::sendSuccessMsg(){
        writeSerialCommand(MSG_TYPE::SUCCESS);
        //serial_thread_reader_->timed_join(boost::chrono::milliseconds(250));
        spinner_sleep(250);
        std::string msg = received_msg_;
        if(!(msg.find("#"+std::to_string(MSG_TYPE::STATE_SUCCESS))!= std::string::npos) && !(msg.find("Last save canceled")!= std::string::npos)){
            output_string_ = "Success msg not recognized";
            //std::cout << output_string_ << std::endl;
            return false;
        }
        output_string_ = "Success msg recognized";
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

    bool ToolFirmwareInterface::saveFirmwareInterfaceConfigFile(std::string _file){

        tool_firmware_interface_config_data_["serial"]["port"] = serial_->getPort();
        tool_firmware_interface_config_data_["serial"]["baud_rate"] = std::to_string(serial_->getBaudRate());
        tool_firmware_interface_config_data_["tool"]["type"] = current_gripper_type_;

        std::ofstream outfile (_file);
        outfile << tool_firmware_interface_config_data_ << std::endl;
        outfile.close();

        return true;
    }

    std::string ToolFirmwareInterface::getCurrentMsg() {
        return received_msg_;
    }

    std::string ToolFirmwareInterface::getOutputString(){
        return output_string_;
    }

}//end namespace