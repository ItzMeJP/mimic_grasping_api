//
// Created by joaopedro on 20/05/21.
//
#include "mimic_grasping_server.h"

using namespace mimic_grasping;

MimicGraspingServer s;

void readCommandCallback(){
    std::cin.ignore();
    std::cout << "ENTER pressed. Leaving..." << std::endl;
    s.stop();

}
int main(){
    std::cout << "\n----------------------------------------------------------" << std::endl;
    std::cout << "------- Starting API. Press ENTER to interrupt.-----------" << std::endl;
    std::cout << "----------------------------------------------------------\n" << std::endl;
    sleep(2);

    boost::thread stop_thread_reader(readCommandCallback);

    if(!s.start()){
        std::cerr << "Mimic Grasping API Error | " << s.getOutputSTR() << std::endl;
        return 0;
    }
    else
        std::cout << "Mimic Grasping API Finished | " << s.getOutputSTR() << std::endl;

    char aux;
    std::cout << "------- SAVE dataset? [y/n].-----------" << std::endl;
    std::cin>> aux;

    if(aux == 'y')
    {
        std::cout << "Exporting the dataset to file..." << std::endl;
        s.exportDatasets();
    }
    else
    {
        std::cout << "No dataset was saved." << std::endl;
    }


    return 0;
}