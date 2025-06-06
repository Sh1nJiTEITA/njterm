cmake_minimum_required(VERSION 4.0.1)
project(njvk)

set(BUILDERS
    "${CMAKE_CURRENT_SOURCE_DIR}/handles/builders/nj_build_instance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handles/builders/nj_build_debug_messanger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handles/builders/nj_build_physical_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handles/builders/nj_build_device.cpp")

add_library(
  njvk STATIC
  ${BUILDERS} "${CMAKE_CURRENT_SOURCE_DIR}/njvk.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/handles/nj_builder.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/handles/nj_vk_instance.cpp")

target_include_directories(njvk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
                                       "${CMAKE_CURRENT_SOURCE_DIR}/handles")

find_package(Vulkan REQUIRED)

target_link_libraries(njvk PRIVATE ${Vulkan_LIBRARY})
target_link_libraries(
  njvk PRIVATE njlog njcon vulkan-memory-allocator::vulkan-memory-allocator)

target_compile_definitions(
  njvk
  PUBLIC APP_NAME="${APP_NAME}"
         APP_VERSION_MAJOR=${APP_VERSION_MAJOR}
         APP_VERSION_MINOR=${APP_VERSION_MINOR}
         APP_VERSION_PATCH=${APP_VERSION_PATCH}
         ENGINE_NAME="${APP_ENGINE_NAME}"
         ENGINE_VERSION_MAJOR=${APP_ENGINE_VERSION_MAJOR}
         ENGINE_VERSION_MINOR=${APP_ENGINE_VERSION_MINOR}
         ENGINE_VERSION_PATCH=${APP_ENGINE_VERSION_PATCH})

if(BUILD_TEST_NJVK)
  # add_executable(njvk_tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/instance.cpp)
  # target_link_libraries(njvk_tests PUBLIC njvk)
  # target_link_libraries(njvk_tests PRIVATE Catch2::Catch2WithMain)
  # target_link_libraries(njvk_tests PRIVATE njlog)
endif()
