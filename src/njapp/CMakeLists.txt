cmake_minimum_required(VERSION 4.0.1)
project(njapp)

set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_app_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nj_app_desc.cpp"
)

add_library(njapp STATIC ${SOURCES})

target_include_directories(njapp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(njapp PRIVATE njpt)
target_link_libraries(njapp PUBLIC njvk)
target_link_libraries(njapp PRIVATE njwin)
target_link_libraries(njapp PRIVATE njcon)
target_link_libraries(njapp PRIVATE njft)
target_link_libraries(njapp PRIVATE njbuf)
