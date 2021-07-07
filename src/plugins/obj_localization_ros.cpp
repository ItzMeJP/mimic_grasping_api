//
// Created by joaopedro on 07/07/21.
//

#include "plugins/obj_localization_ros.h"

ObjLocalizationROS::ObjLocalizationROS(){

}
ObjLocalizationROS::~ObjLocalizationROS(){

}

bool ObjLocalizationROS::setAppConfigPath(std::string _path){
    plugin_config_path_ = _path;
    return true;
}
bool ObjLocalizationROS::setAppExecPath(std::string _path){
    plugin_exec_path_ = _path;
    return true;
}
bool ObjLocalizationROS::runApp(){

    if(!first_obj_localization_communication_){
        obj_localization_thread_reader_->interrupt();
        obj_localization_thread_reader_->join();
        //pclose(pipe_to_obj_localization_);
    }

    if(plugin_exec_path_ == ""){
        output_string_ = plugin_name + " exec is not defined.";
        return false;
    }

    pipe_to_obj_localization_ = popen(plugin_exec_path_.c_str(), "r"); // TODO: find a solution to treat this error!
    if(errno != 0 || pipe_to_obj_localization_ == NULL){
        output_string_ = plugin_name + " cannot exec.";
        return false;
    }

    int descriptor = fileno(pipe_to_obj_localization_);
    fcntl(descriptor, F_SETFL, O_NONBLOCK);

    obj_localization_thread_reader_.reset(new boost::thread (boost::bind(&ObjLocalizationROS::execCallback,this,descriptor)));

    first_obj_localization_communication_ = false;

    return true;
}
bool ObjLocalizationROS::requestData(Pose& _result){

    if(err_flag_pipe_corrupted_){
        output_string_ = "Corrupted pipe to " + plugin_name + " interface.";
        return false;
    }

    // TODO: implement ROS interface here. Just test below.
    _result.setName("test_pose");
    _result.setParentName("object");
    _result.setPosition(Eigen::Translation3d(0,0,0));
    _result.setRPYOrientationZYXOrder(Eigen::Vector3d (0,0,0.78539));

    return true;
}
int ObjLocalizationROS::getStatus(){
    if(err_flag_pipe_corrupted_){
        output_string_ = "Corrupted pipe to " + plugin_name + " interface. ";
        return FEEDBACK::ERROR;
    }
    return FEEDBACK::RUNNING;
}

std::string ObjLocalizationROS::getOutputString(){
    return output_string_;
}

void ObjLocalizationROS::execCallback(int _file_descriptor){
    for(;;) {
        try {
            this->exec(_file_descriptor);
            boost::this_thread::interruption_point();
            //boost::this_thread::sleep(boost::posix_time::milliseconds(500)); //interruption with sleep
        }
        catch(boost::thread_interrupted&)
        {
            std::cout << plugin_name + " pipe thread is stopped." << std::endl;
            return;
        }
    }
}
void ObjLocalizationROS::exec(int _file_descriptor) {

    char buffer[128];

    ssize_t r = read(_file_descriptor, buffer, 128);

    if (r == -1 && errno == EAGAIN) {}
    else if (r > 0) {
        current_pipe_output_str_ = buffer;
    } else{
            //std::cout << "Corrupted pipe to " + plugin_name + " interface. " << std::endl;
            err_flag_pipe_corrupted_ = true;
            return;
    }

}

void ObjLocalizationROS::spin(int _usec){
    obj_localization_thread_reader_->timed_join(boost::chrono::milliseconds(_usec));
    if(err_flag_pipe_corrupted_){
        output_string_ = "Corrupted pipe to " + plugin_name + " interface. ";
        obj_localization_thread_reader_->interrupt();
        obj_localization_thread_reader_->join();
        //pclose(pipe_to_obj_localization_);
        //strerror(errno);
    }
}