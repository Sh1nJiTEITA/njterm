cmake_minimum_required(VERSION 4.0.1)
project(njterm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# CMAKE ADDS
include("cmake/find_dep.cmake")

# TARGETS
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njlog")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njpt")

# EXE
add_executable(njterm ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(njterm PRIVATE njpt)

# DEPENDECIES

# SETTINGS ==================================================

# For LSP server autocomplete
set(CMAKE_BUILD_TYPE Debug)
# CPP-23 standart
set_property(TARGET njterm PROPERTY CXX_STANDART 23)
