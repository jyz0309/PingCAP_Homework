cmake_minimum_required(VERSION 3.15)
project(pingcap_homework)

set(CMAKE_CXX_STANDARD 11)

add_executable(pingcap_homework main.cpp Buffer_Node.h thread_pool.h thread_pool.cpp K_Merge.h Buffer_Node.cpp K_Merge.cpp)
find_package(Threads REQUIRED)
target_link_libraries(pingcap_homework Threads::Threads)