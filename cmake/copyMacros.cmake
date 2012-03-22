# file(COPY ...) will only copy files when running cmake
# this is quite a shortcoming
include(CMakeParseArguments)

# @brief copy_file
# @param DESTINATION destination directory
# @param[optional] RELATIVE (relative path ?)
# @param ARGN files to copy (should works with directories too)
macro(copy_file)
  set(options RELATIVE)
  set(oneValueArgs DESTINATION)
  set(multiValueArgs "")

  cmake_parse_arguments(copy
    "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  foreach(loop_var ${copy_UNPARSED_ARGUMENTS})
    if(copy_RELATIVE)
      set(file "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
    else()
      set(file "${loop_var}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${file}
      ${copy_DESTINATION})
  endforeach()
endmacro()
