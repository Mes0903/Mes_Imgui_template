cmake_minimum_required(VERSION 3.11)
 
project(DETECT_SIMULATION)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${THIRD_PARTY_DIR}/imgui)
add_subdirectory(${THIRD_PARTY_DIR})

aux_source_directory(./src SRC_DIR)
add_executable(DETECT_SIMULATION ${SRC_DIR})

if(WIN32)
  target_link_libraries(
    DETECT_SIMULATION
    IMGUI_LIB
    ${THIRD_PARTY_DIR}/windows/lib/glfw3.lib
    ${THIRD_PARTY_DIR}/windows/lib/OpenGL32.Lib
  )

elseif(UNIX)
  find_package(OpenGL REQUIRED)
  if(OPENGL_FOUND)
    message('OPENGL_FOUND-is-true')
  else()
    message('OPENGL_FOUND-is-false')
  endif()

  target_link_libraries(
    DETECT_SIMULATION
    ${OPENGL_LIBRARIES}
    glfw
    IMGUI_LIB
  )
endif()























# add_custom_target(Makefile COMMAND make
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
# )
