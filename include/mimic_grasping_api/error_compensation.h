//
// Created by joaopedro on 6/28/22.
//

#ifndef MIMIC_GRASPING_API_ERROR_COMPENSATION_H
#define MIMIC_GRASPING_API_ERROR_COMPENSATION_H

#include <string>
#include <fstream>

#include <transform_manipulation/pose.h>

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

#define JSON_X_DISPLACEMENT_TAG "x" //in meters
#define JSON_Y_DISPLACEMENT_TAG "y"
#define JSON_Z_DISPLACEMENT_TAG "z"
#define JSON_ROLL_DISPLACEMENT_TAG "roll" //in degrees
#define JSON_PITCH_DISPLACEMENT_TAG "pitch"
#define JSON_YAW_DISPLACEMENT_TAG "yaw"

#define JSON_TYPE_TAG "type"
#define JSON_DESCRIPTION_TAG "description"

#define JSON_CONST_ABS_OFFSET_TAG "offset"
#define JSON_CONST_REL_PERCENTAGE_TAG "percentage"
#define JSON_EXP_BASE_TAG "base"
#define JSON_EXP_ALPHA_TAG "alpha"
#define JSON_EXP_MULT_TAG "multiplier"
#define JSON_LIN_A_TAG "a"
#define JSON_LIN_B_TAG "b"

namespace mimic_grasping {
    class ErrorCompensation {
    public:

        ErrorCompensation(){};
        ~ErrorCompensation(){};

        enum CORRECTION_TYPE {
            CONST_ABS,
            CONST_RELATIVE,
            LINEAR,
            EXPONENTIAL
        };

        struct errorCompensationData {
            int type;
            double offset = NAN,     //  absolute constant correction
            percentage = NAN, //  relative constant correction
            base = NAN,       //  exponential correction
            alpha = NAN,      //  exponential correction
            multiplier = NAN, //  exponential correction
            a_coef = NAN,     //  linear correction
            b_coef = NAN;     //  linear correction
        };

        bool loadCompensationFile(std::string _file_with_path);

        bool applyCorrection(Pose &_pose);

        bool applyRunTimeLoadCorrection(std::string _file_with_path, Pose &_pose);



    protected:
        Json::Value json_file_;


    private:
        std::string output_string_;

        errorCompensationData x_data_,
                                y_data_,
                                z_data_,
                                roll_data_,
                                pitch_data_,
                                yaw_data_;

        bool loadSpecificErrorCompensation(errorCompensationData &_in_data, std::string _tag_name);
        bool applySpecificErrorCompensation(errorCompensationData _in_data, double &_input);

        static inline double from_degrees(double degrees)
        {
         return degrees * M_PI / 180.0;
        }

        static inline double to_degrees(double radians)
        {
        return radians * 180.0 / M_PI;
        }

/*
        double applyLinearCorrection(double _a, double _b, double _x)
        double applyAbsConstCorrection();
        double applyRelConstCorrection();
        double applyExpCorrection();
*/
    };
}

#endif //MIMIC_GRASPING_API_ERROR_COMPENSATION_H
