#include"LrpcProvider.h"
#include"zookeeperutil.h"
#include "LrpcApplication.h"
#include<zookeeper/zookeeper.h>

void LrpcProvider::NotifyService(google::protobuf::Service* service){
    ServiceInfo info;
    info.service = service;
    const google::protobuf::ServiceDescriptor* serviceDesc = service->GetDescriptor();
    std::string serviceName = serviceDesc->full_name();
    std::cout<<"serviceName:"<<serviceName<<std::endl;
    for(int i=0;i<serviceDesc->method_count();i++){
        std::string methodName = serviceDesc->method(i)->full_name();
        std::cout<<"methodname:"<<methodName<<std::endl;
        info.methodMap.insert({methodName,serviceDesc->method(i)});
    }
    serviceMap.insert({serviceName,info});
}

void LrpcProvider::ZKtest(){
    std::string ip = LrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    std::string port = LrpcApplication::GetInstance().GetConfig().Load("rpcserverport");
    std::string rpcserveraddr = ip + ":" + port;
    ZooClient zkclient;
    zkclient.Start();
    for(auto& service:serviceMap){
        std::string servicePath = "/"+service.first;
        zkclient.Create(servicePath.c_str(),nullptr,0,0);
        for(auto& method:service.second.methodMap){
            std::string methodPath = servicePath + "/" +method.first;
            zkclient.Create(methodPath.c_str(),rpcserveraddr.c_str(),rpcserveraddr.size(),ZOO_EPHEMERAL);
            std::string rpcdata = zkclient.GetData(methodPath.c_str());
            std::cout<<"rpcdata:"<<rpcdata<<std::endl;
        }
    }
}