cmake_minimum_required(VERSION 4.0.1)
project(njvk)

add_library(njvk STATIC "${CMAKE_CURRENT_SOURCE_DIR}/njvk.cpp")

find_package(Vulkan REQUIRED)
target_link_libraries(njvk PRIVATE ${Vulkan_LIBRARY})

find_package(glfw3 REQUIRED)
target_link_libraries(njvk PRIVATE glfw3)

find_package(glm REQUIRED)
target_link_libraries(njvk PRIVATE glm)

target_link_libraries(njvk PUBLIC njlog)


