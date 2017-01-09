cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project (chat)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/proto)

add_subdirectory(proto)
add_subdirectory(client)
add_subdirectory(server)

add_dependencies(client proto)
add_dependencies(server proto)
