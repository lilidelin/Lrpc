#include"LrpcConfig.h"
#include<memory>

//加载配置文件，解析文件中的键值对
void LrpcConfig::LoadConfigFile(const char* config_file){
    std::unique_ptr<FILE, decltype(&fclose)> fp(
        fopen(config_file, "r"), 
        &fclose
    );
    if(fp == nullptr){
        throw std::runtime_error("open config file failed");
        exit(EXIT_FAILURE);
    }
    char buf[1024];
    while(fgets(buf,1024,fp.get())!=nullptr){
        std::string read_buf(buf);
        Trim(read_buf);

        if(read_buf.empty() || read_buf[0] == '#'){
            continue;
        }

        int index = read_buf.find("=");
        if(index == -1){
            continue;
        }

        std::string key = read_buf.substr(0,index);
        int endindex = read_buf.find("\n",index);
        std::string value = read_buf.substr(index+1,endindex-index-1);
        Trim(key);
        Trim(value);
        configMap.insert({key,value});
    }
}

std::string LrpcConfig::Load(const std::string& key){
    auto iter = configMap.find(key);
    if(iter==configMap.end()){
        throw std::runtime_error("key not found in config file");
        exit(EXIT_FAILURE);
    }
    return iter->second;
}

void LrpcConfig::Trim(std::string& read_buf){
    int front = read_buf.find_first_not_of(" ");
    int back = read_buf.find_last_not_of(" ");
    if(front == -1 || back == -1){
        read_buf.clear();
        return;
    }
    read_buf = read_buf.substr(front,back-front+1);
}