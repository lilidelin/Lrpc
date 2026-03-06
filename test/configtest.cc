#include"LrpcApplication.h"
#include<iostream>
using namespace std;
int main(int argc, char* argv[]){
    LrpcApplication::Init(argc, argv);
    LrpcConfig& config = LrpcApplication::GetConfig();
    cout<<config.Load("rpcserverip")<<endl;
    cout<<config.Load("rpcserverport")<<endl;
    cout<<config.Load("zookeeperip")<<endl;
    cout<<config.Load("zookeeperport")<<endl;
    return 0;
}