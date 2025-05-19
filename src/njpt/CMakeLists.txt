cmake_minimum_required(VERSION 4.0.1)
project(njpt)

add_library(njpt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/njpt.cpp)
target_include_directories(njpt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# find_package(njlog REQUIRED)
target_link_libraries(njpt PRIVATE njlog)
