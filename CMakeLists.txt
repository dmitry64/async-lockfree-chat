cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set (PROJECT
        chat)

set (SERVER_HEADERS
        messages/include/chat_message.hpp
        )

set (SERVER_SOURCES
        server/src/server.cpp)

set (CLIENT_HEADERS
        messages/include/chat_message.hpp
        )

set (CLIENT_SOURCES
        client/src/client.cpp)

set (BOOST_COMPONENTS
        thread
        filesystem)

set (Boost_USE_STATIC_LIBS ON)

set (Boost_USE_MULTITHREADED ON)

project (${PROJECT})

add_subdirectory(proto)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/proto)
include_directories (messages/include)
include_directories (server/include)
include_directories (client/include)

find_package (Threads REQUIRED)

find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable (server ${SERVER_HEADERS} ${SERVER_SOURCES})
target_link_libraries (server proto ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARY})
target_compile_features(server PRIVATE cxx_range_for)

add_executable (client ${CLIENT_HEADERS} ${CLIENT_SOURCES})
target_link_libraries (client proto ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARY})
target_compile_features(client PRIVATE cxx_range_for)




