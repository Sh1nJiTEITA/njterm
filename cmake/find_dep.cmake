# find_package(Vulkan REQUIRED) if(Vulkan_FOUND) message(STATUS "Found Vulkan
# with version ${Vulkan_VERSION}") message(STATUS "Vulkan include dirs at
# path=${Vulkan_INCLUDE_DIRS}") endif()
#
# if(Vulkan_glslc_FOUND) message( STATUS "Found Vulkan glslc compiler at
# path=${Vulkan_GLSLC_EXECUTABLE}") endif()
#
# find_package(fmt REQUIRED) if(fmt_FOUND) message(STATUS "Found fmt lib")
# endif()
#
# # find_package(Lua REQUIRED) if(Lua_FOUND) message(STATUS "Found lua lib") #
# endif()
#
find_package(Catch2 REQUIRED)
if(Catch2_FOUND)
  message(STATUS "Found catch2
lib")
endif()

# find_package(vulkan-memory-allocator REQUIRED) if(Catch2_FOUND) message(STATUS
# "Found vulkan-memory-allocator lib") endif()

find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(lua REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
find_package(vulkan-memory-allocator REQUIRED)
