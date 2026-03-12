#include"LrpcApplication.h"
#include"../../user/user.pb.h"
#include"LrpcController.h"
#include"LrpcChannel.h"
#include<iostream>
#include<atomic>
#include<thread>
#include<chrono>

void SendRequest(int thread_id,std::atomic<int>& success_count,std::atomic<int>& fail_count){
    Luser::UserServiceRpc_Stub stub(new LrpcChannel(false));

    Luser::LoginReq request;
    request.set_username("test");
    request.set_password("123456");
    Luser::LoginResp response;
    LrpcController controller;
    stub.Login(&controller,&request,&response, nullptr);
    if(controller.Failed()){
        std::cout<<"Thread "<<thread_id<<" fail: "<<controller.ErrorText()<<std::endl;
        fail_count++;
    }else{
        if(0 == response.result().errcode()){
            std::cout<<"Thread "<<thread_id<<" success: "<<response.success()<<std::endl;
            success_count++;
        }
        else{
            std::cout<<"Thread "<<thread_id<<" fail: "<<response.result().errmsg()<<std::endl;
            fail_count++;
        }
    }
}

int main(int argc,char** argv){
    LrpcApplication::Init(argc, argv);

    const int thread_num = 1;
    const int requests_per_thread = 1;
    std::vector<std::thread> threads;
    std::atomic<int> success_count(0);
    std::atomic<int> fail_count(0);

    auto start_time = std::chrono::high_resolution_clock::now();

    for(int i=0;i<thread_num;i++){
        threads.emplace_back([argc,argv,i,&success_count,&fail_count,requests_per_thread](){
            for(int j=0;j<requests_per_thread;j++){
                SendRequest(i,success_count,fail_count);
            }
        });
    }
    for(auto& t : threads){
        t.join();
    }

    auto EndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = EndTime - start_time;
    std::cout<<"Total requests: "<<thread_num * requests_per_thread<<std::endl;
    std::cout<<"Success: "<<success_count.load()<<std::endl;
    std::cout<<"Fail: "<<fail_count.load()<<std::endl;
    std::cout<<"Time cost: "<<duration.count()<<" s"<<std::endl;
    return 0;
}