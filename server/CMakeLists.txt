cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project (server)

set (SERVER_HEADERS
        include/chatsession.h
        include/chatserver.h
        include/chatroom.h
        include/chatsubscriber.h)

set (SERVER_SOURCES
        src/chatsession.cpp
        src/chatserver.cpp
        src/chatroom.cpp
        src/chatsubscriber.cpp
        src/server.cpp)

set (BOOST_COMPONENTS
        thread
        filesystem)

set (Boost_USE_STATIC_LIBS ON)

set (Boost_USE_MULTITHREADED ON)
set(CMAKE_CXX_FLAGS  -fno-strict-aliasing)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/proto)
include_directories (include)
include_directories(../cds)

find_package (Threads REQUIRED)

find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable (server ${SERVER_HEADERS} ${SERVER_SOURCES})
target_link_libraries (server proto cds ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARY})
target_compile_features(server PRIVATE cxx_range_for)




