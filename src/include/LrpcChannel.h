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
private:
    bool is_secure_;
    int m_clientfd;
    int m_idx;
    std::string service_name;
    std::string method_name;
    bool NewConnect(const char* ip, int port);
    std::string QueryServiceHost(ZooClient& zkclient, const std::string& service_name, const std::string& method_name, int& idx);
};
#endif
