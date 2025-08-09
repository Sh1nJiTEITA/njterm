cmake_minimum_required(VERSION 4.0.1)
project(njterm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# CMAKE ADDS
include("cmake/find_dep.cmake")
include("cmake/compile_shaders.cmake")

# INFO ======================================================
set(APP_NAME "njterm")
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 1)
set(APP_VERSION_PATCH 0)
set(APP_ENGINE_NAME "njtermeng")
set(APP_ENGINE_VERSION_MAJOR 0)
set(APP_ENGINE_VERSION_MINOR 1)
set(APP_ENGINE_VERSION_PATCH 0)

# SETTINGS ==================================================
option(USE_GLFW "Use GLFW as window impl" ON)

# TESTS
option(BUILD_TEST_NJCON "Build tests for njcon" ON)
option(BUILD_TEST_NJVK "Build tests for njvk" ON)

# TARGETS
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njlog")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njpt")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njvk")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njwin")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njcon")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/njft")

# EXE
add_executable(njterm ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(njterm PRIVATE njpt)
target_link_libraries(njterm PRIVATE njvk)
target_link_libraries(njterm PRIVATE njwin)
target_link_libraries(njterm PRIVATE njcon)
target_link_libraries(njterm PRIVATE njft)

target_compile_definitions(
  njterm
  PUBLIC APP_NAME="${APP_NAME}"
         APP_VERSION_MAJOR=${APP_VERSION_MAJOR}
         APP_VERSION_MINOR=${APP_VERSION_MINOR}
         APP_VERSION_PATCH=${APP_VERSION_PATCH}
         ENGINE_NAME="${APP_ENGINE_NAME}"
         ENGINE_VERSION_MAJOR=${APP_ENGINE_VERSION_MAJOR}
         ENGINE_VERSION_MINOR=${APP_ENGINE_VERSION_MINOR}
         ENGINE_VERSION_PATCH=${APP_ENGINE_VERSION_PATCH})

# DEPENDECIES

# For LSP server autocomplete
set(CMAKE_BUILD_TYPE Debug)
# CPP-23 standart
set_property(TARGET njterm PROPERTY CXX_STANDART 23)

set(SHADER_PACK_PATHS "${CMAKE_SOURCE_DIR}/shaders/basic")

compile_shaders(${SHADER_PACK_PATHS})
