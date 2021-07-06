//
// Created by joaopedro on 02/07/21.
//

#include "plugins/foo.h"

Foo::Foo(){

}

Foo::~Foo(){

}

bool Foo::setAppConfigPath(std::string _path){
    return true;
}
bool Foo::setAppExecPath(std::string _path){
    return true;
}
bool Foo::runApp(){
    return true;
}
bool Foo::requestData(Pose& _result){
    _result.setName("test_pose");
    _result.setParentName("object");
    _result.setPosition(Eigen::Translation3d(0,0,0));
    _result.setRPYOrientationZYXOrder(Eigen::Vector3d (0,0,0.78539));
    return true;
}
int Foo::getStatus(){
    return FEEDBACK::FINISHED;
}

