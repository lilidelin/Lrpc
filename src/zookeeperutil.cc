#include"zookeeperutil.h"
#include<mutex>
#include<condition_variable>
#include"LrpcApplication.h"
#include<cstdlib>
#include<iostream>

std::mutex cv_mutex;
std::condition_variable cv;
bool is_connected = false;

ZooClient::ZooClient(){
    zkhandle = nullptr;
}

ZooClient::~ZooClient(){
    if(zkhandle != nullptr){
        zookeeper_close(zkhandle);
    }
}

void global_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx){
    if(type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE){
        std::lock_guard<std::mutex> lock(cv_mutex);
        is_connected = true;
        cv.notify_all();
    }
    else{
        std::cout<<"zookeeper_watcher: type = "<<type<<", state = "<<state<<", path = "<<path<<std::endl;
        exit(EXIT_FAILURE);
    }
}

void ZooClient::Start(){
    std::string ip = LrpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = LrpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string serviceaddr = ip + ":" + port;
    zkhandle = zookeeper_init(serviceaddr.c_str(), global_watcher, 6000, nullptr, nullptr, 0);
    if(zkhandle == nullptr){
        std::cout<<"zookeeper_init failed"<<std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait(lock, []{return is_connected;});
    std::cout<<"zookeeper_init success"<<std::endl;
}

void ZooClient::Create(const char* path, const char* data, int datalen, int flags){
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    if(zoo_exists(zkhandle,path,0,nullptr) == ZNONODE){
        int ret = zoo_create(zkhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,flags,path_buffer,bufferlen);
        if(ret != ZOK){
            std::cout<<"zookeeper_create failed"<<std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout<<"zookeeper_create success, path = "<<path_buffer<<std::endl;
    }
    else{
        std::cout<<"zookeeper_create failed, znode already exists, path = "<<path<<std::endl;
    }
}

std::string ZooClient::GetData(const std::string& path){
    char data_buffer[128];
    int datalen = sizeof(data_buffer);
    int ret = zoo_get(zkhandle,path.c_str(),0,data_buffer,&datalen,nullptr);
    if(ret != ZOK){
        std::cout<<"zookeeper_get failed, path = "<<path<<std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string(data_buffer);
}