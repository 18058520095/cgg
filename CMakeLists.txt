cmake_minimum_required(VERSION 3.16)
project(lcl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

find_package(Threads REQUIRED)

add_library(lcl
    src/file/FileDescriptor.cpp
    src/file/FileIO.cpp
    src/net/SocketAddress.cpp
    src/net/Socket.cpp
    src/net/TcpServer.cpp
    src/net/TcpClient.cpp
    src/thread/TaskQueue.cpp
    src/thread/ThreadPool.cpp
    src/process/ProcessPool.cpp
)
target_include_directories(lcl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lcl PUBLIC Threads::Threads)

set(LCL_APPS
    file_copy
    tcp_echo_server
    tcp_echo_client
    tcp_pool_server
    udp_echo_server
    udp_echo_client
    process_pool_server
)

foreach(app ${LCL_APPS})
    add_executable(${app} apps/${app}.cpp)
    target_link_libraries(${app} PRIVATE lcl)
endforeach()

enable_testing()
add_executable(test_basic tests/test_basic.cpp)
target_link_libraries(test_basic PRIVATE lcl)
add_test(NAME basic COMMAND test_basic)