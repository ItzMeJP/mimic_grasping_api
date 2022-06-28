//
// Created by joaopedro on 05/07/21.
//

#include "localization_interface.h"
namespace mimic_grasping {

    LocalizationInterface::LocalizationInterface() {};

    LocalizationInterface::~LocalizationInterface() {
        //stopObjLocalization();
        //stopToolLocalization();
        //obj_localization_obj_.reset();
        //tool_localization_obj_.reset();
        //clearPluginInstances();
    };

    bool LocalizationInterface::saveLocalizationConfigFile(std::string _file) {

        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_OBJ_ONESHOOT_TAG] = obj_localization_data_.one_shoot_estimation_;
        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG] = obj_localization_data_.executor;
        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_TERM_CMD_TAG] = obj_localization_data_.terminator;
        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG] = obj_localization_data_.plugin_name;
        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_SPEC_CONFIG_FILE_TAG] = obj_localization_data_.specific_configuration_file;
        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_APPLY_CORRECTION_TAG] = obj_localization_data_.apply_error_compensation;
        localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_ERR_CORRECTION_FILE_TAG] = obj_localization_data_.error_compensation_file;

        localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG] = tool_localization_data_.executor;
        localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_TERM_CMD_TAG] = tool_localization_data_.terminator;
        localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG] = tool_localization_data_.plugin_name;
        localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_SPEC_CONFIG_FILE_TAG] = tool_localization_data_.specific_configuration_file;
        localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_APPLY_CORRECTION_TAG] = tool_localization_data_.apply_error_compensation;
        localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_ERR_CORRECTION_FILE_TAG] = tool_localization_data_.error_compensation_file;

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
            output_string_ = "Localization configuration file not found. Current path: " + _file;
            return false;
        }

        ToolLocalizationData aux_tool_data;
        aux_tool_data.plugin_name = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_PL_NAME_TAG].asString();
        aux_tool_data.executor = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_EX_CMD_TAG].asString();
        aux_tool_data.terminator = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_TERM_CMD_TAG].asString();
        aux_tool_data.specific_configuration_file = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_SPEC_CONFIG_FILE_TAG].asString();
        aux_tool_data.apply_error_compensation = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_APPLY_CORRECTION_TAG].asBool();
        aux_tool_data.error_compensation_file = localization_interface_config_data_[JSON_TOOL_LOC_TAG][JSON_ERR_CORRECTION_FILE_TAG].asString();
        setToolLocConfig(aux_tool_data);

        ObjLocalizationData aux_obj_data;
        aux_obj_data.plugin_name = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_PL_NAME_TAG].asString();
        aux_obj_data.executor = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_EX_CMD_TAG].asString();
        aux_obj_data.terminator = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_TERM_CMD_TAG].asString();
        aux_obj_data.specific_configuration_file = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_SPEC_CONFIG_FILE_TAG].asString();
        aux_obj_data.apply_error_compensation = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_APPLY_CORRECTION_TAG].asBool();
        aux_obj_data.one_shoot_estimation_ = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_OBJ_ONESHOOT_TAG].asBool();
        aux_obj_data.error_compensation_file = localization_interface_config_data_[JSON_OBJ_LOC_TAG][JSON_ERR_CORRECTION_FILE_TAG].asString();
        setObjLocConfig(aux_obj_data);

        return true;
    }

    bool LocalizationInterface::setLocalizationConfigsFolderPath(std::string _path) {
        profile_folder_path_ = _path;
        return true;
    }

    bool LocalizationInterface::setLocalizationScriptsFolderPath(std::string _path) {
        scripts_folder_path_ = _path;
        return true;
    }
/*
    std::string LocalizationInterface::getLocalizationOutputSTR() {
        return output_string_;
    }
*/
    bool LocalizationInterface::setObjLocalizationTarget(std::string _target) {
        obj_localization_obj_->setTargetName(_target);
        return true;
    }

    bool LocalizationInterface::setToolLocalizationTarget(std::string _target) {
        tool_localization_obj_->setTargetName(_target);
        return true;
    }

    std::string LocalizationInterface::getObjLocalizationTarget() {
        return obj_localization_obj_->getTargetName();
    }

    std::string LocalizationInterface::getToolLocalizationTarget() {
        return tool_localization_obj_->getTargetName();
    }

    bool LocalizationInterface::initToolLocalization() {
        return (initLocalization(tool_localization_obj_, tool_localization_data_));
    }

    bool LocalizationInterface::initObjLocalization() {

        return (initLocalization(obj_localization_obj_, obj_localization_data_));
    }

    void LocalizationInterface::clearPluginInstances() {
        obj_localization_obj_.reset();
        tool_localization_obj_.reset();
        ClearPluginList();
    }


    bool
    LocalizationInterface::initLocalization(std::shared_ptr<LocalizationBase> &_loc_instance, LocalizationData _data) {

        int plugin_index = (getIndexOfPlugin(_data.plugin_name)),
                class_index = 0;

        if (plugin_index < 0) {
            output_string_ = "Cannot init the localization. There is no plugin \"" + _data.plugin_name +
                             "\" into the loaded plugin list.";
            return false;
        }

        _loc_instance.reset();
        _loc_instance = CreateInstanceAs<LocalizationBase>(plugin_index,
                                                           class_index); //it is supposed to have only one class

        if (_loc_instance == nullptr) {
            output_string_ = "Cannot instantiate the localization for " +
                             std::string(GetPluginFactoryInfo(plugin_index)->GetClassName(class_index)) + " class.";
            return false;
        }

        std::string ex_cmd = "", term_cmd = "", config_path = "", error_compensation_path = "";
        ex_cmd = isScript(_data.executor) ? scripts_folder_path_ + "/" + _data.executor : _data.executor;
        term_cmd = isScript(_data.terminator) ? scripts_folder_path_ + "/" + _data.terminator : _data.terminator;
        config_path = profile_folder_path_ + "/" + _data.specific_configuration_file;
        error_compensation_path = profile_folder_path_ + "/" + _data.error_compensation_file;

        if (!_loc_instance->setAppExec(ex_cmd)
            || !_loc_instance->setAppTermination(term_cmd)
            || !_loc_instance->setAppConfigPath(config_path)) {

            output_string_ = _loc_instance->getOutputString();
            return false;
        }

        if (!_loc_instance->loadAppConfiguration() || !_loc_instance->runApp()) {
            output_string_ = _loc_instance->getOutputString();
            return false;
        }

        _loc_instance->spin(1000);
        output_string_ = _loc_instance->getOutputString();

        if (_loc_instance->getStatus() == LocalizationBase::FEEDBACK::ERROR) {
            return false;
        }

        if(loadCompensationFile(error_compensation_path)){
            output_string_ = "Failed to load error compensation file.";
            return false;
        }

        return true;
    }

    bool LocalizationInterface::stopObjLocalization() {
        return obj_localization_obj_->stopApp();
    }

    bool LocalizationInterface::stopToolLocalization() {
        return tool_localization_obj_->stopApp();
    }

    bool LocalizationInterface::requestObjPose(Pose &_pose) {

        bool aux = obj_localization_obj_->requestData(_pose);

        if(!aux || !obj_localization_data_.apply_error_compensation)
            return aux;

        else
        {
            // The correction is applied every request. This allow runtime error compensations by just modifying the config file
            std::string error_compensation_path = profile_folder_path_ + "/" + obj_localization_data_.error_compensation_file;
            std::cout << "Applying correction in object pose. " << std::endl;
            applyRunTimeLoadCorrection(error_compensation_path,_pose);
        }
    }

    bool LocalizationInterface::requestToolPose(Pose &_pose) {

        bool aux = tool_localization_obj_->requestData(_pose);

        if(!aux || !tool_localization_data_.apply_error_compensation)
            return aux;

        else
        {
            std::string error_compensation_path = profile_folder_path_ + "/" + tool_localization_data_.error_compensation_file;
            std::cout << "Applying correction in tool pose. " << std::endl;
            applyRunTimeLoadCorrection(error_compensation_path,_pose);
        }


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

    std::string LocalizationInterface::getLocalizationInterfaceOutputSTR() {
        return output_string_;
    }

    bool LocalizationInterface::object_localization_spinner_sleep(int usec) {

        obj_localization_obj_->spin(int(usec));
        if (obj_localization_obj_->getStatus() == LocalizationBase::ERROR) {
            output_string_ = obj_localization_obj_->getOutputString();
            return false;
        }
        /*else if (obj_localization_obj_->getStatus() == LocalizationBase::ABORTED) {
            output_string_ = obj_localization_obj_->getOutputString();
        }
         */

        return true;
    }

    bool LocalizationInterface::tool_localization_spinner_sleep(int usec) {

        tool_localization_obj_->spin(int(usec));
        if (tool_localization_obj_->getStatus() == LocalizationBase::ERROR) {
            output_string_ = tool_localization_obj_->getOutputString();
            return false;
        }

        return true;
    }

    LocalizationInterface::ObjLocalizationData LocalizationInterface::getObjLocLoadedConfig(){
        return obj_localization_data_;
    }

    LocalizationInterface::ToolLocalizationData LocalizationInterface::getToolLocLoadedConfig(){
        return tool_localization_data_;
    }

    void LocalizationInterface::setObjLocConfig(ObjLocalizationData _in){
        _in.plugin_name = isDynamicLib(_in.plugin_name)? removeDLExtension(_in.plugin_name ) : _in.plugin_name;
        std::cout << "nome "<< isDynamicLib(_in.plugin_name) << "---" << removeDLExtension(_in.plugin_name ) << "---- "<< _in.plugin_name <<std::endl;
        obj_localization_data_ = _in;
    }

    void LocalizationInterface::setToolLocConfig(ToolLocalizationData _in){
        _in.plugin_name = isDynamicLib(_in.plugin_name)? removeDLExtension(_in.plugin_name ) : _in.plugin_name;
        std::cout << "nome "<< isDynamicLib(_in.plugin_name) << "---" << removeDLExtension(_in.plugin_name ) << "---- "<< _in.plugin_name <<std::endl;

        tool_localization_data_ = _in;
    }

    bool LocalizationInterface::isScript(std::string _s) {

        std::string script_extension = ".sh";

        if (_s.find(script_extension) != std::string::npos)
            return true;

        return false;
    }

    bool LocalizationInterface::isDynamicLib(std::string _s){
        std::string script_extension = ".so";

        if (_s.find(script_extension) != std::string::npos)
            return true;

        return false;
    }

    std::string LocalizationInterface::removeDLExtension(std::string _s){
        std::string delimiter = ".so", token;
        if(!isDynamicLib(_s)){
            output_string_ = "The input string " + _s + " does not have a .so extension";
            return _s;
        }

        token = _s.substr(0,_s.find(delimiter));
        std::cout << "input string " << _s << std::endl;

        std::cout << "token " << token<< std::endl;

        return token;
    }

    bool LocalizationInterface::isOneShoot(){
        return obj_localization_data_.one_shoot_estimation_;
    }

    bool LocalizationInterface::isToolLocCompensated(){
        return tool_localization_data_.apply_error_compensation;
    }

    bool LocalizationInterface::isObjLocCompensated(){
        return obj_localization_data_.apply_error_compensation;
    }
}