cmake_minimum_required(VERSION 3.13)
project(ToyRedis)

set(CMAKE_CXX_STANDARD 17)

#add_executable(ToyRedis main.cpp Src/sds.cpp Src/sds.h)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        master
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

add_subdirectory(Src)
add_subdirectory(UnitTest)