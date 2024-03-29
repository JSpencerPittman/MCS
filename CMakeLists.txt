cmake_minimum_required(VERSION 3.22)

project(MCS)

set (CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

add_subdirectory(externals/googletest)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

if(WIN32)
    message(STATUS "Building for Windows")
	file(GLOB_RECURSE PLATFORM_SRC_FILES src/udp_windows.cpp)
endif (WIN32)

if(UNIX OR APPLE)
	message(STATUS "Building for UNIX")
	file(GLOB_RECURSE PLATFORM_SRC_FILES src/udp_unix.cpp)
endif(UNIX OR APPLE)

file(GLOB GENERAL_SRC_FILES src/stream_protocol.cpp src/rtp.cpp)

add_executable(Client src/client.cpp ${GENERAL_SRC_FILES} ${PLATFORM_SRC_FILES})
add_executable(Server src/server.cpp ${GENERAL_SRC_FILES} ${PLATFORM_SRC_FILES})
add_executable(Dev src/dev.cpp ${GENERAL_SRC_FILES} ${PLATFORM_SRC_FILES})

target_link_libraries(Client ${OpenCV_LIBS})
target_link_libraries(Server ${OpenCV_LIBS})
target_link_libraries(Dev ${OpenCV_LIBS})

enable_testing()
file(GLOB TEST_SRC_FILES test/*)
add_executable(Test ${TEST_SRC_FILES} ${GENERAL_SRC_FILES} ${PLATFORM_SRC_FILES})
target_link_libraries(Test ${OpenCV_LIBS} GTest::gtest_main)