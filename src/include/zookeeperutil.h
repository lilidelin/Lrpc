#ifndef ZOOKEEPERUTIL_H
#define ZOOKEEPERUTIL_H

#include<string>
#include<zookeeper/zookeeper.h>

class ZooClient{
    public:
        ZooClient();
        ~ZooClient();
        void Start();
        void Create(const char* path, const char* data, int datalen, int flags);
        std::string GetData(const std::string& path);

    private:
        zhandle_t* zkhandle;
};

#endif
