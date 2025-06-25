cmake_minimum_required(VERSION 4.0.1)
project(njvk)

set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_builder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_instance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_debug_utils_messenger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_physical_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_swapchain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_render_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_render_sync.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_render_pass.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_grid_render_pass.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_attachment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_command_pool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_command_buffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_attachment_color.cpp")

add_library(njvk STATIC ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/njvk.cpp")

# target_compile_definitions(njvk PUBLIC VULKAN_HPP_NO_SMART_HANDLE=0)
# target_compile_definitions(njvk PUBLIC VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1)

target_include_directories(
  njvk
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/handles")

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
