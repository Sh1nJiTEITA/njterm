cmake_minimum_required(VERSION 4.0.1)
project(njlog)

add_library(njlog SHARED ${CMAKE_CURRENT_SOURCE_DIR}/njlog.cpp)
target_include_directories(njlog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

find_package(fmt REQUIRED)
target_link_libraries(njlog INTERFACE fmt::fmt)
