find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
  message(STATUS "Found Vulkan with version ${Vulkan_VERSION}")
  message(STATUS "Vulkan include dirs at path=${Vulkan_INCLUDE_DIRS}")
endif()

if(Vulkan_glslc_FOUND)
  message(
    STATUS "Found Vulkan glslc compiler at path=${Vulkan_GLSLC_EXECUTABLE}")
endif()

find_package(fmt REQUIRED)
if(fmt_FOUND)
  message(STATUS "Found fmt lib")
endif()
