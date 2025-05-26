cmake_minimum_required(VERSION 4.0.1)
project(njcon)

add_library(njcon SHARED ${CMAKE_CURRENT_SOURCE_DIR}/njcon.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/njlua.cpp)
target_include_directories(njcon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(njcon PRIVATE njlog)
target_link_libraries(njcon PRIVATE ${LUA_LIBRARIES})
target_include_directories(njcon PRIVATE ${LUA_INCLUDE_DIR})

if(BUILD_TEST_NJCON)

  add_executable(njcon_tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_state.cpp)
  target_link_libraries(njcon_tests PUBLIC njcon)
  target_link_libraries(njcon_tests PRIVATE Catch2::Catch2WithMain)

endif()
