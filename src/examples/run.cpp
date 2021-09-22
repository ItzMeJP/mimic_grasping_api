//
// Created by joaopedro on 20/05/21.
//
#include "mimic_grasping_server.h"

using namespace mimic_grasping;

std::shared_ptr<MimicGraspingServer> s = std::make_shared<MimicGraspingServer>() ;

std::string removeRootTimestamp(std::string _str){

    std::string delimiter = ": ", token;
    if(!(_str.find(delimiter)!= std::string::npos)){ //there is no timestamp
        return _str;
    }
    else{
        token = _str.substr(_str.find(delimiter)+delimiter.length(), std::string::npos);
        return token;
    }

}

bool strsAreEqual(std::string &_new_str, std::string &_old_str ){

    _new_str = removeRootTimestamp(_new_str);
    _old_str = removeRootTimestamp(_old_str);

    return _new_str == _old_str;

}

void appendTextWithoutRepetition(std::string _qstr){

    std::string new_str;
    static std::string old_str = "";

    new_str = _qstr;
    if(!strsAreEqual(new_str,old_str)){
        old_str = new_str;
        std::cout<< new_str << std::endl;
    }

    return;
}


void readCommandCallback(){
    std::cin.ignore();
    std::cout << "ENTER pressed. Leaving..." << std::endl;
    s->request_stop();

}

void printOutputCallback(){
    for (;;) {
        try {
            appendTextWithoutRepetition( s->getOutputSTR());
            boost::this_thread::interruption_point();
            boost::this_thread::sleep(boost::posix_time::milliseconds(5)); //interruption with sleep
        }
        catch (boost::thread_interrupted &) {
            return;
        }
    }
}


int main(int argc, char *argv[ ] ){

    if(argc == 1)
        s->setProfile("default");
    else{
        s->setProfile(argv[1]);
    }

    std::cout << "\n----------------------------------------------------------" << std::endl;
    std::cout << "------ Starting API. Press ENTER twice to interrupt.------" << std::endl;
    std::cout << "----------------------------------------------------------\n" << std::endl;
    sleep(2);

    boost::thread stop_thread_reader(readCommandCallback);
    boost::thread output_thread_printer(printOutputCallback);


    if(!s->start()){
        std::cerr << "Mimic Grasping API Error | " << s->getErrorStr() << std::endl;
        return 0;
    }
    else
        std::cout << "Mimic Grasping API Finished | " << s->getOutputSTR() << std::endl;

    char aux;
    std::cout << "------- SAVE dataset? [Y/N].-----------" << std::endl;
    std::cin >> aux;

    if(aux == 'y'|| aux == 'Y')
    {
        std::cout << "Exporting the dataset to file..." << std::endl;
        if(!s->exportDatasets())
        {
            std::cerr << "Mimic Grasping API Error | " << s->getErrorStr() << std::endl;
        }
    }
    else
    {
        std::cout << "None dataset were saved." << std::endl;
    }

    //The issue bellow is related to ROS plugins... The Action service is not able to be called twice...
/*
    s.reset(new MimicGraspingServer());


    if(!s->start()){
        std::cerr << "Mimic Grasping API Error | " << s->getErrorStr() << std::endl;
        return 0;
    }
    else
        std::cout << "Mimic Grasping API Finished | " << s->getOutputSTR() << std::endl;


    std::cout << "------- SAVE dataset? [Y/N].-----------" << std::endl;
    std::cin >> aux;

    if(aux == 'y'|| aux == 'Y')
    {
        std::cout << "Exporting the dataset to file..." << std::endl;
        if(!s->exportDatasets())
        {
            std::cerr << "Mimic Grasping API Error | " << s->getErrorStr() << std::endl;
        }
    }
    else
    {
        std::cout << "None dataset were saved." << std::endl;
    }

*/



// JUST to test











    output_thread_printer.interrupt();
    output_thread_printer.join();
    return 0;
}