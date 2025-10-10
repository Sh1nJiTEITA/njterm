cmake_minimum_required(VERSION 4.0.1)
project(njimg)

add_library(njimg SHARED "${CMAKE_CURRENT_SOURCE_DIR}/nj_img.cpp")
target_include_directories(njimg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")

find_package(stb REQUIRED)
target_link_libraries(njimg PUBLIC stb::stb)


