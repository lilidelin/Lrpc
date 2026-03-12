#ifndef _LrpcChannel_h_
#define _LrpcChannel_h_
#include<google/protobuf/service.h>
#include"zookeeperutil.h"
class LrpcChannel : public google::protobuf::RpcChannel{
public:
    LrpcChannel(bool connect_now);
    ~LrpcChannel();
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                    google::protobuf::Message* response, google::protobuf::Closure* done) override;
};


#endif
