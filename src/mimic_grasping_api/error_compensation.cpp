//
// Created by joaopedro on 6/28/22.
//

#include "error_compensation.h"

namespace mimic_grasping {


bool ErrorCompensation::loadCompensationFile(std::string _file_with_path) {

    std::ifstream config_file(_file_with_path, std::ifstream::binary);

    if (config_file) {
        try {
            config_file >> json_file_;
        } catch (const std::exception &e) {
            output_string_ = e.what();
            DEBUG_MSG("Loading compensation file exception error: " << output_string_);
            return false;
        }
    } else {
        output_string_ = "Error compensation files does not found. Current path: \" + _file";
        DEBUG_MSG(output_string_);
        return false;
    }


    loadSpecificErrorCompensation(x_data_,JSON_X_DISPLACEMENT_TAG);
    loadSpecificErrorCompensation(y_data_,JSON_Y_DISPLACEMENT_TAG);
    loadSpecificErrorCompensation(z_data_,JSON_Z_DISPLACEMENT_TAG);
    loadSpecificErrorCompensation(roll_data_,JSON_ROLL_DISPLACEMENT_TAG);
    loadSpecificErrorCompensation(pitch_data_,JSON_PITCH_DISPLACEMENT_TAG);
    loadSpecificErrorCompensation(yaw_data_,JSON_YAW_DISPLACEMENT_TAG);


    return true;
}

    bool ErrorCompensation::loadSpecificErrorCompensation(errorCompensationData &_in_data, std::string _tag_name){

        _in_data.type = json_file_[_tag_name][JSON_TYPE_TAG].asInt();



        switch (_in_data.type) {
            case CORRECTION_TYPE::CONST_ABS:
                _in_data.offset = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_CONST_ABS_OFFSET_TAG].asDouble();
                break;
            case CORRECTION_TYPE::CONST_RELATIVE:
                _in_data.percentage = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_CONST_REL_PERCENTAGE_TAG].asDouble();
                break;
            case CORRECTION_TYPE::LINEAR:
                _in_data.a_coef = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_LIN_A_TAG].asDouble();
                _in_data.b_coef = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_LIN_B_TAG].asDouble();
                break;
            case CORRECTION_TYPE::EXPONENTIAL:
                _in_data.base = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_EXP_BASE_TAG].asDouble();
                _in_data.alpha = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_EXP_ALPHA_TAG].asDouble();
                _in_data.multiplier = json_file_[_tag_name][JSON_DESCRIPTION_TAG][JSON_EXP_MULT_TAG].asDouble();
                break;
        }
    return true;
}

    bool ErrorCompensation::applySpecificErrorCompensation(errorCompensationData _in_data, double &input){

        DEBUG_MSG("Input Raw: " << input);



        switch (_in_data.type) {
            case CORRECTION_TYPE::CONST_ABS:
                input += _in_data.offset;
                break;
            case CORRECTION_TYPE::CONST_RELATIVE:
                input += (_in_data.percentage*input)/100;
                break;
            case CORRECTION_TYPE::LINEAR:
                input += _in_data.a_coef*input+_in_data.b_coef;
                break;
            case CORRECTION_TYPE::EXPONENTIAL:
                input += _in_data.multiplier*pow(_in_data.base,(_in_data.alpha*input));
                break;
        }

        DEBUG_MSG("Input Corrected: " << input);

        return true;

}
/*
    double  ErrorCompensation::applyLinearCorrection(double _a, double _b, double _input){

        double err;

        err = _a*_input+_b;
        err = abs(err);


        err
        return y;
    }



    double ErrorCompensation::applyAbsConstCorrection(){
        double r;
    return r;
    }

    double ErrorCompensation::applyRelConstCorrection(){
        double r;
    return r;
    }

    double ErrorCompensation::applyExpCorrection(){
        double r;
    return r;
    }
*/

    bool ErrorCompensation::applyRunTimeLoadCorrection(std::string _file_with_path,Pose &_pose){

        if(!loadCompensationFile(_file_with_path) ||
            !applyCorrection(_pose))
            return false;

        return true;

    }

    bool ErrorCompensation::applyCorrection(Pose &_pose) {

        output_string_ = "Applying correction. ";
        DEBUG_MSG(output_string_);

        Eigen::Translation3d t = _pose.getPosition();
        Eigen::Vector3d r = _pose.getRPYOrientationZYXOrder();

        applySpecificErrorCompensation(x_data_,t.x());
        applySpecificErrorCompensation(y_data_,t.y());
        applySpecificErrorCompensation(z_data_,t.z());
        applySpecificErrorCompensation(roll_data_,r.x());
        applySpecificErrorCompensation(pitch_data_,r.y());
        applySpecificErrorCompensation(yaw_data_,r.z());

        _pose.setPosition(t);
        _pose.setRPYOrientationZYXOrder(r);

    return true;
}



}