cmake_minimum_required(VERSION 3.11)
 
project(IMGUI_DEMO)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  message('OPENGL_FOUND-is-true')
else()
  message('OPENGL_FOUND-is-false')
endif()

set(THIRD_DIR ${PROJECT_SOURCE_DIR}/3rdparty)

include_directories(
  ${THIRD_DIR}/imgui
  ${THIRD_DIR}/implot
  ${OPENGL_INCLUDE_DIRS}
)

add_subdirectory(${THIRD_DIR})

add_executable(
  IMGUI_DEMO 
  ${PROJECT_SOURCE_DIR}/src/imgui_demo.cpp
)

target_link_libraries(
  IMGUI_DEMO
  IMGUI_LIB
  IMPLOT_LIB
  $<$<BOOL:${WIN32}>:${THIRD_DIR}/windows/lib/glfw3.lib>
  $<$<BOOL:${UNIX}>:glfw>
  ${OPENGL_LIBRARIES}
)