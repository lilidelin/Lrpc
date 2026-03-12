#include"LrpcChannel.h"
#include<google/protobuf/descriptor.h>
void LrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                    google::protobuf::Message* response, google::protobuf::Closure* done){
    std::cout<<"CallMethod: "<<method->full_name()<<std::endl;
}

LrpcChannel::LrpcChannel(bool is_secure){
    std::cout<<"LrpcChannel: "<<is_secure<<std::endl;
}

LrpcChannel::~LrpcChannel(){
    std::cout<<"~LrpcChannel"<<std::endl;
}