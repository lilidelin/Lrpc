#include"LrpcChannel.h"
#include"LrpcHeader.pb.h"
#include<google/protobuf/descriptor.h>
#include<string>
#include<mutex>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"LrpcLogger.h"

std::mutex g_data_mutx;
void LrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                    google::protobuf::Message* response, google::protobuf::Closure* done){
    if(-1 == m_clientfd){
        const google::protobuf::ServiceDescriptor* service = method->service();
        service_name = service->name();
        method_name = method->name();

        ZooClient zkclient;
        zkclient.Start();

        std::string host_data = QueryServiceHost(zkclient, service_name,method_name,m_idx);

        if(host_data.empty()){
            LOG_ERROR("QueryServiceHost: %s:%s failed",service_name.c_str(),method_name.c_str());
            return;
        }
        std::string m_ip = host_data.substr(0,m_idx);
        int m_port = atoi(host_data.substr(m_idx+1).c_str());
        LOG_INFO("QueryServiceHost: %s:%d",m_ip.c_str(),m_port);
        auto flag = NewConnect(m_ip.c_str(),m_port);
        if(!flag){
            LOG_ERROR("NewConnect: %s:%d failed",m_ip.c_str(),m_port);
            return;
        }
        LOG_INFO("NewConnect: %s:%d success",m_ip.c_str(),m_port);
    }

    uint32_t args_size{};
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();
    }
    else{
        LOG_ERROR("CallMethod: SerializeToString failed");
        return;
    }

    Lrpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    std::string header_str;
    if(!rpc_header.SerializeToString(&header_str)){
        LOG_ERROR("CallMethod: SerializeToString failed");
        return;
    }
    uint32_t header_size = header_str.size();

    std::string send_rpc_str;
    {
        google::protobuf::io::StringOutputStream string_stream(&send_rpc_str);
        google::protobuf::io::CodedOutputStream coded_stream(&string_stream);
        coded_stream.WriteVarint32(header_size);
        coded_stream.WriteString(header_str);
    }
    send_rpc_str += args_str;

    if(-1 == send(m_clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)){
        close(m_clientfd);
        char err_msg[128];
        sprintf(err_msg,"send failed, errno: %d",errno);
        LOG_ERROR("%s",err_msg);
        controller->SetFailed(err_msg);
        return;
    }

    char recv_rpc_str[1024];
    int recv_size = recv(m_clientfd,recv_rpc_str,1024,0);
    if(recv_size == -1){
        close(m_clientfd);
        char err_msg[128];
        sprintf(err_msg,"recv failed, errno: %d",errno);
        LOG_ERROR("%s",err_msg);
        controller->SetFailed(err_msg);
        return;
    }
    if(!response->ParseFromArray(recv_rpc_str,recv_size)){
        close(m_clientfd);
        char err_msg[128];
        sprintf(err_msg,"ParseFromArray failed, errno: %d",errno);
        LOG_ERROR("%s",err_msg);
        controller->SetFailed(err_msg);
        return;
    }
    close(m_clientfd);
}

LrpcChannel::LrpcChannel(bool is_secure):m_clientfd(-1),m_idx(0){
    LOG_INFO("LrpcChannel: %d",is_secure);
}

LrpcChannel::~LrpcChannel(){
    LOG_INFO("~LrpcChannel");
}

std::string LrpcChannel::QueryServiceHost(ZooClient& zkclient, const std::string& service_name, const std::string& method_name, int& idx){
    std::string path = "/" + service_name + "/" + method_name;
    LOG_INFO("QueryServiceHost: %s",path.c_str());
    std::unique_lock<std::mutex> lock(g_data_mutx);
    std::string host_data = zkclient.GetData(path);
    if(host_data.empty()){
        LOG_ERROR("QueryServiceHost: %s failed",path.c_str());
        return "";
    }
    else{
        LOG_INFO("QueryServiceHost: %s",host_data.c_str());
    }
    idx = host_data.find(":");
    if(idx == -1){
        LOG_ERROR("QueryServiceHost: %s failed, no ':' found",path.c_str());
        return "";
    }
    return host_data;
}

bool LrpcChannel::NewConnect(const char* ip, int port){
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(clientfd == -1){
        LOG_ERROR("socket failed");
        return false;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if(connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){         
        close(clientfd);
        LOG_ERROR("connect failed");
        return false;
    }   
    m_clientfd = clientfd;
    return true;
}