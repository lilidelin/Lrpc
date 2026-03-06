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
)

set(SOURCES
    test/configtest.cc
    ./src/LrpcApplication.cc
    ./src/LrpcConfig.cc
)

add_executable(configtest ${SOURCES})

target_link_libraries(configtest ${PROTOBUF_LIBRARIES})
