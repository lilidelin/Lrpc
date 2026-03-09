#ifndef LRPC_PROVIDER_H
#define LRPC_PROVIDER_H

#include<unordered_map>
#include<string>
#include"google/protobuf/service.h"
#include<google/protobuf/descriptor.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<functional>

class LrpcProvider{
public:
    LrpcProvider() = default;
    ~LrpcProvider();
    void NotifyService(google::protobuf::Service* service);
    void ZKtest();
    void Run();
private:
    struct ServiceInfo{
        google::protobuf::Service* service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> methodMap;
    };
    std::unordered_map<std::string,ServiceInfo> serviceMap;
    muduo::net::EventLoop event_loop;

    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp time);
    void SendResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
};

#endif
