#ifndef LRPC_CONTROLLER_H
#define LRPC_CONTROLLER_H
#include<google/protobuf/service.h>
#include<string>
class LrpcController: public google::protobuf::RpcController{
public:
    LrpcController();
    ~LrpcController();
    void Reset() override;
    bool Failed() const override;
    std::string ErrorText() const override;
    void SetFailed(const std::string& reason) override;
    void StartCancel() override;
    bool IsCanceled() const override;
    void NotifyOnCancel(google::protobuf::Closure* callback) override;

private:
    bool m_failed;
    std::string m_errorText;
    bool m_canceled;
};


#endif
