#include <mimic_grasping_server/mimic_grasping_server.h>

namespace mimic_grasping {

    MimicGraspingServer::MimicGraspingServer(){
        serial_ = std::make_shared<SimpleSerial>("/dev/ttyUSB0",115200);
        //serial_thread_reader_ = std::make_shared<boost::thread>(&MimicGraspingServer::readCommandCallback, this);
    }

    MimicGraspingServer::~MimicGraspingServer(){

    }

    void MimicGraspingServer::start() {

        output_string_ = "Null";
        if(!startToolCommunication(output_string_))
            return;

        std::string command = "";
        for(;;) {

            std::cout << "Current msg: " << received_msg_ << std::endl;
            std::cin>>command;
            writeSerialCommand(command);

            if(command == "-1")
            {
                std::cout<< "Changing config" << std::endl;
                if(!setSerialConfig("/dev/ttyUSB0",115200,output_string_))
                    return;
            }
            if(command == "-99")
            {
                std::cout<< "Changing gripper type to TWO F" << std::endl;
                if(!setGripperType(GRIPPER_TYPE::PARALLEL_PNEUMATIC_TWO_FINGER))
                    return;
            }
            if(command == "-100")
            {
                std::cout<< "Changing gripper type to SINGLE SUC" << std::endl;
                if(!setGripperType(GRIPPER_TYPE::SINGLE_SUCTION_CUP))
                    return;
            }

        }

    }

    bool MimicGraspingServer::setGripperType(int _gripper)
    {
        writeSerialCommand(MSG_TYPE::RESET);
        //boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        writeSerialCommand(_gripper);
        serial_thread_reader_->timed_join(boost::chrono::milliseconds(2500));

        std::string r = received_msg_;

        std::string token = r.substr(0, r.find(firmware_state_delimiter_));
        r.erase(0, 0 + token.length());


        std::cout << "Token: " << r << " -------"<< std::endl;
        return true;
    }

    std::string MimicGraspingServer::getOutputString(){
        return output_string_;
    }
}//end namespace