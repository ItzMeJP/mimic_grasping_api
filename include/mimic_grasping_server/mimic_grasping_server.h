#ifndef MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
#define MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H

#include <iostream>
#include <thread>

#include <simple_serial/simple_serial.h>
#include <boost/thread.hpp>


namespace mimic_grasping{

    class MimicGraspingServer{

    public:
        MimicGraspingServer();
        ~MimicGraspingServer();

        bool setSerialConfig(std::string _port, unsigned int _baud_rate, std::string &_output_str);
        void writeSerialCommand(std::string _command);
        void writeSerialCommand(int _s);
        std::string readCommand();
        bool startToolCommunication(std::string &_output_str);
        std::string getOutputString();
        bool setGripperType(int _gripper);

        void start();

        enum MSG_TYPE
        {
            CONNECTION_STABILISHED_TWO_ALTERNATE_RELAYS = 99,
            CONNECTION_STABILISHED_ONE_RELAY = 100,
            ERROR = 101,
            SUCCESS = 102,
            RESET = 103,
            SAVE = 104,
            REMOVE_LAST_SAVE = 105
        };

        enum GRIPPER_TYPE{
            PARALLEL_PNEUMATIC_TWO_FINGER = 99,
            SINGLE_SUCTION_CUP = 100
        };

    private:

        std::shared_ptr<SimpleSerial> serial_;
        void readCommandCallback();
        //std::shared_ptr<std::thread> serial_thread_reader_;
        std::shared_ptr<boost::thread> serial_thread_reader_;

        std::string output_string_,
                    received_msg_,
                    firmware_state_delimiter_ = " >> ";

        bool first_tool_communication = true;


    }; // end class

} // end namespace


#endif //MIMIC_GRASPING_SERVER_MIMIC_GRASPING_SERVER_H
