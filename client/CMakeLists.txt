cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project (client)

set (CLIENT_HEADERS
        include/chatclient.h)

set (CLIENT_SOURCES
        src/chatclient.cpp
        src/client.cpp)

set (BOOST_COMPONENTS
        thread
        filesystem)

set (Boost_USE_STATIC_LIBS ON)

set (Boost_USE_MULTITHREADED ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories (include)

find_package (Threads REQUIRED)

find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable (client ${CLIENT_HEADERS} ${CLIENT_SOURCES})
target_link_libraries (client proto ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARY})
target_compile_features(client PRIVATE cxx_range_for)




