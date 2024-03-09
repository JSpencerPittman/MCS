cmake_minimum_required(VERSION 3.22)

project(MCS)

set(CMAKE_CXX_STANDARD_REQUIRED 20)

include_directories(include)

file(GLOB SRC_FILES src/udp.cpp)

add_executable(Client src/client.cpp ${SRC_FILES})
add_executable(Server src/server.cpp ${SRC_FILES})
