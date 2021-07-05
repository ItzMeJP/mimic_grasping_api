//
// Created by joaopedro on 20/05/21.
//
#include "mimic_grasping_server/mimic_grasping_server.h"

using namespace mimic_grasping;

int main(){
    MimicGraspingServer s;
    s.start();
    std::cout << "Output: " << s.getOutputSTR() << std::endl;
    return 0;
}