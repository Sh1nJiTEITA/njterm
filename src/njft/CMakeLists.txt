cmake_minimum_required(VERSION 4.0.1)
project(njft)

add_library(njft STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_ft_library.cpp")
target_include_directories(njft PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# target_include_directories( njwin PUBLIC
# "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" PRIVATE
# "${CMAKE_CURRENT_SOURCE_DIR}/internal")

target_link_libraries(njft PRIVATE njlog)

find_package(freetype REQUIRED)
target_link_libraries(njft PUBLIC Freetype::Freetype)

find_package(Fontconfig REQUIRED)
target_link_libraries(njft PUBLIC Fontconfig::Fontconfig)
