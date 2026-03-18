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
    ./example/callee/Lservice.cc
    ./src/LrpcController.cc
    ./src/LrpcLogger.cc
)

add_executable(server ${SOURCES})

target_link_libraries(server ${PROTOBUF_LIBRARIES})
target_link_libraries(server zookeeper_mt)
target_link_libraries(server muduo_net muduo_base pthread)
