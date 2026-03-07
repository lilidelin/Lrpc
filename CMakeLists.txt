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
    ./user/user.pb.h
)

set(SOURCES
    ./src/LrpcApplication.cc
    ./src/LrpcConfig.cc
    ./src/LrpcProvider.cc
    ./example/callee/Lservice.cc
    ./user/user.pb.cc
)

add_executable(notifyservice_test ${SOURCES})

target_link_libraries(notifyservice_test ${PROTOBUF_LIBRARIES})
