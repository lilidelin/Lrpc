#include"LrpcController.h"

LrpcController::LrpcController(){
    m_failed = false;
    m_errorText = "";
    m_canceled = false;
}
LrpcController::~LrpcController(){

}
void LrpcController::Reset() {
    m_failed = false;
    m_errorText = "";
    m_canceled = false;
}
bool LrpcController::Failed() const {
    return m_failed;
}
std::string LrpcController::ErrorText() const {
    return m_errorText;
}
void LrpcController::SetFailed(const std::string& reason) {
    m_failed = true;
    m_errorText = reason;
}
void LrpcController::StartCancel() {
    m_canceled = true;
}
bool LrpcController::IsCanceled() const {
    return m_canceled;
}
void LrpcController::NotifyOnCancel(google::protobuf::Closure* callback) {
}
