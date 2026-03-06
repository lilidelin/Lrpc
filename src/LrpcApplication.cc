#include"LrpcApplication.h"
#include<cstdlib>
#include<unistd.h>
#include<iostream>

LrpcConfig LrpcApplication::m_config;
std::mutex LrpcApplication::m_mutex;
LrpcApplication* LrpcApplication::my_application = nullptr;

void LrpcApplication::Init(int argc, char* argv[]){
    if(argc< 2){
        std::cout<<"格式： command -i <配置文件路径>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    int o;
    std::string config_file_path;
    while((o=getopt(argc, argv, "i:"))!=-1){
        switch(o){
            case 'i':
                config_file_path = optarg;
                break;
            case '?':
                std::cout<<"格式： command -i <配置文件路径>"<<std::endl;
                exit(EXIT_FAILURE);
            case ':':
                std::cout<<"格式： command -i <配置文件路径>"<<std::endl;
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
    m_config.LoadConfigFile(config_file_path.c_str());
}

LrpcApplication& LrpcApplication::GetInstance(){
    std::lock_guard<std::mutex> lock(m_mutex);
    if(my_application == nullptr){
        my_application = new LrpcApplication();
        atexit(deleteInstance);
    }
    return *my_application;
}

void LrpcApplication::deleteInstance(){
    if(my_application){
        delete my_application;
        my_application = nullptr;
    }
}

LrpcConfig& LrpcApplication::GetConfig(){
    return m_config;
}