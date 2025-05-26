cmake_minimum_required(VERSION 4.0.1)
project(njwin)

add_library(njwin STATIC "${CMAKE_CURRENT_SOURCE_DIR}/njwin.cpp")
target_include_directories(
  njwin
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/internal")

if(USE_GLFW)
  message(STATUS "Using GLFW as window impl")
  target_sources(
    njwin PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/internal/njglfw.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/internal/njglfw.cpp")
  target_compile_definitions(njwin PRIVATE NJ_USE_IMPL_GLFW)
else()
  message(ERROR "No window impl are chosen")
endif()

find_package(glfw3 REQUIRED)
target_link_libraries(njwin PRIVATE glfw)

find_package(glm REQUIRED)
target_link_libraries(njwin PRIVATE glm::glm)
target_link_libraries(njwin PRIVATE njlog)
