cmake_minimum_required(VERSION 3.10)

project(protobuf_demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)

if(NOT Protobuf_FOUND)
    message(FATAL_ERROR "Protobuf library not found")
endif()

include_directories(
    ${PROTOBUF_INCLUDE_DIRS}
    ./src/include
    ./user
)

set(SOURCES
    ./src/LrpcApplication.cc
    ./src/LrpcConfig.cc
    ./src/LrpcProvider.cc
    ./user/user.pb.cc
    ./src/zookeeperutil.cc
    ./src/LrpcHeader.pb.cc
    ./src/LrpcChannel.cc
    ./example/caller/LClient.cc
    ./src/LrpcController.cc
)

add_executable(client_test ${SOURCES})

target_link_libraries(client_test ${PROTOBUF_LIBRARIES})
target_link_libraries(client_test zookeeper_mt)
target_link_libraries(client_test muduo_net muduo_base pthread)
