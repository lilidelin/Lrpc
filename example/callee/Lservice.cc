#include<iostream>
#include<string>
#include"../../user/user.pb.h"
#include <google/protobuf/service.h>   // RpcController, Closure
#include <google/protobuf/stubs/common.h> // for PROTOBUF_NAMESPACE_ID
#include "LrpcProvider.h"
#include "LrpcApplication.h"

class UserService : public Luser::UserServiceRpc{
public:
    bool Login(std::string name,std::string pwd){
        std::cout<<"Login called!"<<std::endl;
        std::cout<<"name:"<<name<<" pwd:"<<pwd<<std::endl;
        return true;
    }

    // use the real namespace (or keep the macro if you included the header
    // that defines it)
    void Login(::google::protobuf::RpcController* controller,
               const ::Luser::LoginReq* request,
               ::Luser::LoginResp* response,
               ::google::protobuf::Closure* done) override
    {
        std::string name = request->username();
        std::string pwd = request->password();

        bool result = Login(name,pwd);
        Luser::ResultCode* code = response->mutable_result();
        response->set_success(result);
        code->set_errcode(0);
        code->set_errmsg("success");
        done->Run();
    }
};
int main(int argc, char** argv){
    LrpcApplication::Init(argc, argv);
    LrpcProvider provider;
    provider.NotifyService(new UserService());
    return 0;
}