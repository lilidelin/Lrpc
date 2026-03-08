#ifndef LRPC_PROVIDER_H
#define LRPC_PROVIDER_H

#include<unordered_map>
#include<string>
#include"google/protobuf/service.h"
#include<google/protobuf/descriptor.h>

class LrpcProvider{
public:
    LrpcProvider() = default;
    ~LrpcProvider() = default;
    void NotifyService(google::protobuf::Service* service);
    void ZKtest();
private:
    struct ServiceInfo{
        google::protobuf::Service* service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> methodMap;
    };
    std::unordered_map<std::string,ServiceInfo> serviceMap;
};

#endif
