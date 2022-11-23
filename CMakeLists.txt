cmake_minimum_required(VERSION 3.11)
 
project(IMGUI_DEMO)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${THIRD_PARTY_DIR}/imgui)
include_directories(${THIRD_PARTY_DIR}/implot)
add_subdirectory(${THIRD_PARTY_DIR})

add_executable(
  IMGUI_DEMO 
  ${PROJECT_SOURCE_DIR}/src/imgui_demo.cpp
)

if(WIN32)
  target_link_libraries(
    IMGUI_DEMO
    IMGUI_LIB
    IMPLOT_LIB
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
    IMGUI_DEMO
    IMGUI_LIB
    IMPLOT_LIB
    glfw
    ${OPENGL_LIBRARIES}
  )

endif()























# add_custom_target(Makefile COMMAND make
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
# )
