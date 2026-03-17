#include"LrpcLogger.h"

void LrpcLogger::SetLogLevel(int level){
    log_level = level;
}
LrpcLogger::LrpcLogger(){
    std::thread writeLogTask([&](){
        time_t now = time(nullptr);
        tm* nowtm = localtime(&now);
        char file_name[128];
        snprintf(file_name, sizeof(file_name), "log/lrpc_%d_%d_%d.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
        pf = fopen(file_name,"a+");
        if(pf == nullptr){
            std::cout<<"open log file failed"<<std::endl;
            return;
        }
        while(true){
            std::string msg = "";
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock,[&](){
                return !log_queue.empty();
            });
            msg = log_queue.front();
            log_queue.pop();
            lock.unlock();
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);
            char time_buf[128];
            snprintf(time_buf, sizeof(time_buf), "%d:%d:%d => [%s]", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec, 
                (log_level == INFO ? "INFO" : (log_level == ERROR ? "ERROR" : (log_level == FATAL ? "FATAL" : "DEBUG")))
        );
            msg.insert(0, time_buf);
            fprintf(pf, "%s\n", msg.c_str());
            fflush(pf);
        }
    });
    writeLogTask.detach();
}
LrpcLogger::~LrpcLogger(){
    if(pf != nullptr){
        fclose(pf);
    }
}
void LrpcLogger::Log(const std::string& msg){
    std::unique_lock<std::mutex> lock(mtx);
    log_queue.push(msg);
    cv.notify_one();
}
LrpcLogger& LrpcLogger::GetInstance(){
    static LrpcLogger instance;
    return instance;
}