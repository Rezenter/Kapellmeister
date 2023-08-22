cmake_minimum_required(VERSION 3.21)
project(GTS-core)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_ARCH "-m64")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${BUILD_ARCH} -Wall -Wextra" )
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${BUILD_ARCH}")



include_directories(${CMAKE_SOURCE_DIR} include)

add_executable(kapellmeister src/server.cpp
        src/mongoose.c
        include/mongoose.h src/RequestHandler.cpp include/RequestHandler.h include/json.hpp src/Mirror.cpp include/Mirror.h)
target_link_libraries(kapellmeister ws2_32)
