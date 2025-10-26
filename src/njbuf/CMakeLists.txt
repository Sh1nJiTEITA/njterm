cmake_minimum_required(VERSION 4.0.1)
project(njbuf)

add_library(
  njbuf SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/nj_text_buffer.cpp)

target_include_directories(njbuf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

find_package(Boost)
target_link_libraries(njbuf PRIVATE njlog Boost::boost)

if(BUILD_TEST_NJCON)

  # add_executable(
  #   njcon_tests
  #   ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_state.cpp
  #   ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_push.cpp
  #   ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_con.cpp)
  #
  # target_link_libraries(njcon_tests PUBLIC njcon)
  # target_link_libraries(njcon_tests PRIVATE Catch2::Catch2WithMain)
  # target_link_libraries(njcon_tests PRIVATE njlog)

endif()
