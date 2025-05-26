cmake_minimum_required(VERSION 4.0.1)
project(njterm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# CMAKE ADDS
include("cmake/find_dep.cmake")

# SETTINGS ==================================================
option(USE_GLFW "Use GLFW as window impl" ON)

# TESTS
option(BUILD_TEST_NJCON "Build tests for njcon" ON)

# TARGETS
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njlog")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njpt")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njvk")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njwin")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njcon")

# EXE
add_executable(njterm ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(njterm PRIVATE njpt)
target_link_libraries(njterm PRIVATE njvk)
target_link_libraries(njterm PRIVATE njwin)

# DEPENDECIES

# SETTINGS ==================================================

# For LSP server autocomplete
set(CMAKE_BUILD_TYPE Debug)
# CPP-23 standart
set_property(TARGET njterm PROPERTY CXX_STANDART 23)
