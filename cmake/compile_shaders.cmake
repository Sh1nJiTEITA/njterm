function(compile_shaders SHADER_DIRS)
  set(VULKAN_SHADER_COMPILER "/usr/bin/glslc")
  add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
  if(EXISTS ${VULKAN_SHADER_COMPILER})
    set(SHADER_EXTENSIONS
        "*.vert"
        "*.frag"
        "*.geom"
        "*.glsl"
        "*.comp"
        "*.tesse"
        "*.tessc")

    set(ALL_SHADER_FILES "")
    foreach(SHADER_DIR_PATH ${SHADER_PACK_PATHS})

      set(ALL_LOCAL_SHADER_FILES "")

      foreach(SHADER_EXT ${SHADER_EXTENSIONS})

        file(GLOB_RECURSE SHADER_FILES_TEMP "${SHADER_DIR_PATH}/${SHADER_EXT}")
        list(APPEND ALL_LOCAL_SHADER_FILES ${SHADER_FILES_TEMP})

      endforeach()

      set(LOCAL_MESSAGE
          "-- [SHADER_PATH_SEARCH] Found shader files in dir ${SHADER_DIR_PATH}:"
      )

      foreach(SHADER_FILE ${ALL_LOCAL_SHADER_FILES})
        # message(STATUS ${SHADER_FILE})
        get_filename_component(FILE_NAME ${SHADER_FILE} NAME)
        string(APPEND LOCAL_MESSAGE " ${FILE_NAME}")
      endforeach()

      message(${LOCAL_MESSAGE})
      list(APPEND ALL_SHADER_FILES ${ALL_LOCAL_SHADER_FILES})
    endforeach()

    foreach(SHADER_FILE ${ALL_SHADER_FILES})
      get_filename_component(NEW_FILE_EXT ${SHADER_FILE} EXT)
      string(REPLACE "." "" NEW_FILE_EXT ${NEW_FILE_EXT})
      get_filename_component(NEW_FILE_DIR ${SHADER_FILE} DIRECTORY)

      get_filename_component(LAST_FOLDER_NAME ${NEW_FILE_DIR} NAME)
      file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})

      set(NEW_FILE_DIR
          ${CMAKE_CURRENT_SOURCE_DIR}/build/shaders/${LAST_FOLDER_NAME})

      execute_process(
        COMMAND
          sh -c
          "${VULKAN_SHADER_COMPILER} ${SHADER_FILE} -o ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE command_result
        OUTPUT_VARIABLE command_output)

      string(ASCII 27 ESC)  # ESC char

      set(GREEN "${ESC}[32m")
      set(RED   "${ESC}[31m")
      set(YELLOW "${ESC}[33m")
      set(RESET "${ESC}[0m")

      if(command_result EQUAL 0)
        message(
            "-- ${GREEN}[SHADER_COMPILE_SUCCESS]${RESET} Shader file ${SHADER_FILE} was compiled to ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv"
        )
      else()
        message(
            "-- ${RED}[SHADER_COMPILE_ERROR]${RESET} Shader compile failed with error code: ${command_result}"
        )
        message("-- ${RED}[SHADER_COMPILE_ERROR]${RESET} Shader output: ${command_output}")
      endif()

      # file(COPY ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv DESTINATION
      # ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})

    endforeach()
  else()
    message(
      FATAL_ERROR
        "Vulkan shader compiler wasn't found at path ${VULKAN_SHADER_COMPILER}")
  endif()
endfunction()
