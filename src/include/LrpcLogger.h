#ifndef _LrpcLogger_h_
#define _LrpcLogger_h_
#include<mutex>
#include<condition_variable>
#include<string>
#include<iostream>
#include<queue>
#include<cstdio>
#include<thread>
#include<time.h>

#define INFO 0
#define ERROR 1
#define FATAL 2
#define DEBUG 3

class LrpcLogger{
public:
    void SetLogLevel(int level);
    void Log(const std::string& msg);
    static LrpcLogger& GetInstance();
    ~LrpcLogger();
private:
    LrpcLogger();
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> log_queue;
    int log_level;
    FILE* pf {nullptr};
};
#endif
