cmake_minimum_required(VERSION 4.0.1)
project(njvk)

add_library(njvk STATIC "${CMAKE_CURRENT_SOURCE_DIR}/njvk.cpp")
target_include_directories(njvk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Vulkan REQUIRED)
target_link_libraries(njvk PRIVATE ${Vulkan_LIBRARY})

