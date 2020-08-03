cmake_minimum_required(VERSION 3.13)

project(TRTest)

include_directories(../Src)

message(STATUS Find gtest Source Dir)
message(STATUS ${googletest_SOURCE_DIR})
include_directories(${googletest_SOURCE_DIR})

add_executable(ToyRedisTest TestRedis.h TestRedis.cpp)

target_link_libraries(ToyRedisTest gtest gtest_main ToyRedisDS)