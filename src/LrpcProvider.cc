#include"LrpcProvider.h"
#include"zookeeperutil.h"
#include "LrpcApplication.h"
#include<zookeeper/zookeeper.h>
#include"LrpcHeader.pb.h"
#include<iostream>
#include<cstdio>
#include<iomanip>
#include"LrpcLogger.h"

using namespace std;

void LrpcProvider::NotifyService(google::protobuf::Service* service){
    ServiceInfo info;
    info.service = service;
    const google::protobuf::ServiceDescriptor* serviceDesc = service->GetDescriptor();
    std::string serviceName = serviceDesc->name();
    LOG_INFO("Register serviceName:%s",serviceName.c_str());
    for(int i=0;i<serviceDesc->method_count();i++){
        std::string methodName = serviceDesc->method(i)->name();
        LOG_INFO("Register methodName:%s",methodName.c_str());
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

void LrpcProvider::Run(){
    LOG_INFO("Run: start");
    std::string ip = LrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    int port = std::stoi(LrpcApplication::GetInstance().GetConfig().Load("rpcserverport"));

    muduo::net::InetAddress address(ip,port);

    std::shared_ptr<muduo::net::TcpServer> server=std::make_shared<muduo::net::TcpServer>(&event_loop,address,"LrpcProvider");

    server->setConnectionCallback(std::bind(&LrpcProvider::OnConnection,this,std::placeholders::_1));
    server->setMessageCallback(std::bind(&LrpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

    server->setThreadNum(4);

    LOG_INFO("Run: before zkclient.Start()");
    ZooClient zkclient;
    zkclient.Start();
    LOG_INFO("Run: after zkclient.Start(), serviceMap size = %zu", serviceMap.size());

    for(auto& service:serviceMap){
        LOG_INFO("Run: registering service: %s", service.first.c_str());
        std::string servicePath = "/" + service.first;
        zkclient.Create(servicePath.c_str(),nullptr,0,0);
        for(auto& method:service.second.methodMap){
            std::string methodPath = servicePath + "/" +method.first;
            char method_path_data[128]={0};
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);
            zkclient.Create(methodPath.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);
        }
    }

    LOG_INFO("LrpcProvider start serving at ip:%s port:%d",ip.c_str(),port);
    server->start();
    event_loop.loop();
}

void LrpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn){
    if(!conn->connected()){
        conn->shutdown();
    }
}

void LrpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp time){
    LOG_INFO("OnMessage called!");
    std::string recv_buf = buffer->retrieveAllAsString();

    google::protobuf::io::ArrayInputStream raw_input(recv_buf.data(),recv_buf.size());
    google::protobuf::io::CodedInputStream coded_input(&raw_input);

    uint32_t header_size{};
    coded_input.ReadVarint32(&header_size);
    LOG_INFO("header_size:%d",header_size);

    std::string rpc_header_str;
    Lrpc::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size{};

    google::protobuf::io::CodedInputStream::Limit msg_limit = coded_input.PushLimit(header_size);
    coded_input.ReadString(&rpc_header_str,header_size);
    LOG_INFO("rpc_header_str size:%zu content:",rpc_header_str.size());
    for(unsigned char c : rpc_header_str){
        std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)c<<" ";
    }
    std::cout<<std::dec<<std::endl;

    coded_input.PopLimit(msg_limit);

    if(rpc_header.ParseFromString(rpc_header_str)){
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
        LOG_INFO("service_name:%s method_name:%s args_size:%d",service_name.c_str(),method_name.c_str(),args_size);
    }
    else{
        LOG_ERROR("rpc_header_str parse failed!");
        return;
    }
    
    std::string args_str;
    bool read_args_succ = coded_input.ReadString(&args_str,args_size);
    if(!read_args_succ){
        LOG_ERROR("read args_str failed!");
        return;
    }

    auto it = serviceMap.find(service_name);
    if(it == serviceMap.end()){
        LOG_ERROR("service_name:%s not found!",service_name.c_str());
        return;
    }

    auto method_it = it->second.methodMap.find(method_name);
    if(method_it == it->second.methodMap.end()){
        LOG_ERROR("method_name:%s not found!",method_name.c_str());
        return;
    }

    google::protobuf::Service* service = it->second.service;
    const google::protobuf::MethodDescriptor* method = method_it->second;
    
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)){
        LOG_ERROR("args_str parse failed!");
        return;
    }

    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    google::protobuf::Closure* done = google::protobuf::NewCallback<LrpcProvider,
                                                                    const muduo::net::TcpConnectionPtr&,
                                                                    google::protobuf::Message*>(this,
                                                                                                &LrpcProvider::SendResponse,
                                                                                                conn,
                                                                                                response);

    service->CallMethod(method, nullptr, request, response, done);

}

void LrpcProvider::SendResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(&response_str)){
        conn->send(response_str);
    }
    else{
        LOG_ERROR("response_str serialize failed!");
    }
}

LrpcProvider::~LrpcProvider(){
    event_loop.quit();
}