//
// Created by joaopedro on 05/07/21.
//

#include <mimic_grasping_server/localization_interface.h>
#include "mimic_grasping_server/localization_interface.h"


LocalizationInterface::LocalizationInterface() {};

LocalizationInterface::~LocalizationInterface() {
    stopObjLocalization();
    stopToolLocalization();
    obj_localization_obj_.reset();
    tool_localization_obj_.reset();
};

bool LocalizationInterface::saveLocalizationConfigFile(std::string _file) {

    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG] = tool_localization_data_.plugin_name;
    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG] = tool_localization_data_.executor;
    localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_TERM_CMD_TAG] = tool_localization_data_.terminator;

    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG] = obj_localization_data_.plugin_name;
    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG] = obj_localization_data_.executor;
    localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_TERM_CMD_TAG] = obj_localization_data_.terminator;

    std::ofstream outfile(_file);
    outfile << localization_interface_config_data_ << std::endl;
    outfile.close();

    return true;
}

bool LocalizationInterface::loadLocalizationConfigFile(std::string _file) {

    std::ifstream config_file(_file, std::ifstream::binary);
    if (config_file) {
        try {
            config_file >> localization_interface_config_data_;
        } catch (const std::exception &e) {
            //std::cerr << e.what() << std::endl;
            output_string_ = e.what();
            return false;
        }
    } else {
        output_string_ = "Localization configuration file not found.";
        return false;
    }

    tool_localization_data_.plugin_name = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG].asString();
    tool_localization_data_.executor = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG].asString();
    tool_localization_data_.terminator = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_TERM_CMD_TAG].asString();
    tool_localization_data_.specific_configuration_file = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_SPEC_CONFIG_FILE_TAG].asString();

    obj_localization_data_.plugin_name = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG].asString();
    obj_localization_data_.executor = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG].asString();
    obj_localization_data_.terminator = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_TERM_CMD_TAG].asString();
    obj_localization_data_.specific_configuration_file = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_SPEC_CONFIG_FILE_TAG].asString();

    return true;
}

bool LocalizationInterface::setLocalizationConfigsFolderPath(std::string _path){
    config_folder_path_ = _path;
    return true;
}

bool LocalizationInterface::setLocalizationScriptsFolderPath(std::string _path){
    scripts_folder_path_ = _path;
    return true;
}

std::string LocalizationInterface::getLocalizationOutputSTR() {
    return output_string_;
}

bool LocalizationInterface::setObjLocalizationTarget(std::string _target){
    obj_localization_obj_->setTargetName(_target);
    return true;
}

bool LocalizationInterface::setToolLocalizationTarget(std::string _target){
    tool_localization_obj_->setTargetName(_target);
    return true;
}

bool LocalizationInterface::initToolLocalization() {
    return (initLocalization(tool_localization_obj_,tool_localization_data_));
}

bool LocalizationInterface::initObjLocalization() {

    return (initLocalization(obj_localization_obj_,obj_localization_data_));
}

bool LocalizationInterface::initLocalization(std::shared_ptr<LocalizationBase>& _loc_instance, LocalizationData _data){

    int plugin_index = (getIndexOfPlugin(_data.plugin_name)),
        class_index = 0;

    if(plugin_index < 0){
        output_string_ = "Cannot init the object localization. There is no plugin \""+ _data.plugin_name +"\" into the loaded plugin list.";
        return false;
    }

    _loc_instance.reset();
    _loc_instance = CreateInstanceAs<LocalizationBase>(plugin_index,class_index); //it is supposed to have only one class

    if(_loc_instance == nullptr){
        output_string_ = "Cannot instantiate the object localization for " + std::string(GetPluginFactoryInfo(plugin_index)->GetClassName(class_index)) + " class.";
        return false;
    }

    std::string ex_cmd, term_cmd, config_path;
    ex_cmd = isScript(_data.executor)?scripts_folder_path_+"/"+_data.executor:_data.executor;
    term_cmd = isScript(_data.terminator)?scripts_folder_path_+"/"+_data.terminator:_data.terminator;
    config_path = config_folder_path_ + "/" + _data.specific_configuration_file;

    if(    !_loc_instance->setAppExec(ex_cmd)
           || !_loc_instance->setAppTermination(term_cmd)
           || !_loc_instance->setAppConfigPath(config_path)
           || !_loc_instance->loadAppConfiguration()
           || !_loc_instance->runApp()){

        output_string_ = _loc_instance->getOutputString();

        return false;

    }
    _loc_instance->spin(1000);
    output_string_ = _loc_instance->getOutputString();

    if(_loc_instance->getStatus() == LocalizationBase::FEEDBACK::ERROR){
        return false;
    }

    return true;
}

bool LocalizationInterface::stopObjLocalization(){
    return obj_localization_obj_->stopApp();
}

bool LocalizationInterface::stopToolLocalization(){
    return tool_localization_obj_->stopApp();
}

bool LocalizationInterface::requestObjPose(Pose& _pose){
    return obj_localization_obj_->requestData(_pose);
}

bool LocalizationInterface::setObjLocalizationTarget(std::string _target_name_with_path){
    current_obj_localization_target_ = _target_name_with_path;
    return true;
}
bool LocalizationInterface::setToolLocalizationTarget(std::string _target_name_with_path){
    current_tool_localization_target_ = _target_name_with_path;
    return true;
}


/*
std::string LocalizationInterface::execIt(const char *cmd, float _startup_delay_seconds) {

    float timeout = 0, deadline = _startup_delay_seconds;
    char buffer[128];
    std::string result;
    FILE *pipe = popen(cmd, "r");
    int descriptor = fileno(pipe);
    fcntl(descriptor, F_SETFL, O_NONBLOCK);

    const clock_t begin_time = clock();

    while (timeout < deadline) {
        timeout = float(clock() - begin_time) / CLOCKS_PER_SEC;
        ssize_t r = read(descriptor, buffer, 128);

        if (r == -1 && errno == EAGAIN) {}
        else if (r > 0) {
            result += buffer;
        } else
            std::cout << "Pipe closed. " << std::endl;
    }

    return result;
}
*/

std::string LocalizationInterface::getLocalizationInterfaceOutputSTR(){
    return output_string_;
}

bool LocalizationInterface::localization_spinner_sleep(int usec){

    obj_localization_obj_->spin(int(usec*0.5));
    if(obj_localization_obj_->getStatus() == LocalizationBase::ERROR){
        output_string_ = obj_localization_obj_->getOutputString();
        return false;
    }

    /* TODO: added tool localization
    tool_localization_obj_->spin(int(usec*0.5));
    if(tool_localization_obj_->getStatus() == LocalizationBase::ERROR){
        output_string_ = tool_localization_obj_->getOutputString();
        return false;
    }
    */
    return true;
}

bool LocalizationInterface::isScript(std::string _s){

    std::string script_extension = ".sh";

    if (_s.find(script_extension) != std::string::npos)
        return true;

    return false;
}