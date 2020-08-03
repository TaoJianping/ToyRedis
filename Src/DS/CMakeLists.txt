cmake_minimum_required(VERSION 3.13)
project(TRDataStruct)

set(CMAKE_CXX_STANDARD 17)

set(
        HEADER
        sds.h
)

set(
        SOURCE
        sds.cpp
)

add_library(ToyRedisDS STATIC ${HEADER} ${SOURCE})
