#include"LrpcProvider.h"

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