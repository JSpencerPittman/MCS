cmake_minimum_required(VERSION 3.22)

project(MCS)

set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

file(GLOB SRC_FILES src/udp.cpp src/stream_protocol.cpp)

add_executable(Client src/client.cpp ${SRC_FILES})
add_executable(Server src/server.cpp ${SRC_FILES})
add_executable(Dev src/dev.cpp ${SRC_FILES})

target_link_libraries(Client ${OpenCV_LIBS})
target_link_libraries(Server ${OpenCV_LIBS})
target_link_libraries(Dev ${OpenCV_LIBS})