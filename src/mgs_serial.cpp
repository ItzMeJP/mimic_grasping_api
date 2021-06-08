#include <mimic_grasping_server/mimic_grasping_server.h>

namespace mimic_grasping {

    bool MimicGraspingServer::setSerialConfig(std::string _port, unsigned int _baud_rate, std::string &_output_str){

        serial_->setPort(_port);
        serial_->setBaudRate(_baud_rate);
        //serial_thread_reader_ = std::make_shared<boost::thread>(boost::bind(&MimicGraspingServer::readCommandCallback, this));

        return startToolCommunication(_output_str);

    }

    bool MimicGraspingServer::startToolCommunication(std::string &_output_str) {

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
        serial_thread_reader_.reset(new boost::thread (boost::bind(&MimicGraspingServer::readCommandCallback, this)));

        first_tool_communication = false;
        return true;
    }


    void MimicGraspingServer::writeSerialCommand(std::string _s){
        serial_->writeString(_s);
    }

    void MimicGraspingServer::writeSerialCommand(int _s){
        serial_->writeString(std::to_string(_s));
    }

    std::string MimicGraspingServer::readCommand(){
        return serial_->readLine();
    }

    void MimicGraspingServer::readCommandCallback(){
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

}//end namespace

