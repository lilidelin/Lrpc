#ifndef _LrpcApplication_h_
#define _LrpcApplication_h_
#include"LrpcConfig.h"
#include<mutex>

class LrpcApplication{
public:
    static void Init(int argc, char* argv[]);
    static LrpcApplication& GetInstance();
    static void deleteInstance();
    static LrpcConfig& GetConfig();

private:
    static LrpcConfig m_config;
    static LrpcApplication* my_application;
    static std::mutex m_mutex;
    LrpcApplication() = default;
    ~LrpcApplication() = default;
    LrpcApplication(const LrpcApplication&) = delete;
    LrpcApplication(LrpcApplication&&) = delete;
};
#endif