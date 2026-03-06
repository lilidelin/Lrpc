#ifndef _LrpcConfig_h_
#define _LrpcConfig_h_
#include<unordered_map>
#include<string>

//这个文件用于加载配置信息，包含以下功能，1.加载配置文件，2查找key对应的value，3.去掉字符串前后空格

class LrpcConfig{
public:
    void LoadConfigFile(const char* config_file);
    std::string Load(const std::string& key);

private:
    std::unordered_map<std::string,std::string> configMap;
    void Trim(std::string& read_buf);
}