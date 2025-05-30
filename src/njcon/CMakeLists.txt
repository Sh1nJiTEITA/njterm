cmake_minimum_required(VERSION 4.0.1)
project(njcon)

add_library(
  njcon SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/njcon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/njluastate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/njluavalue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/njluautils.cpp)

target_include_directories(njcon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(njcon PRIVATE njlog)
target_link_libraries(njcon PRIVATE lua::lua)

if(BUILD_TEST_NJCON)

  add_executable(njcon_tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_state.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_push.cpp)

  target_link_libraries(njcon_tests PUBLIC njcon)
  target_link_libraries(njcon_tests PRIVATE Catch2::Catch2WithMain)
  target_link_libraries(njcon_tests PRIVATE njlog)

endif()
