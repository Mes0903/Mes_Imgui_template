file(GLOB IMGUI_INCLUDE CONFIGURE_DEPENDS 
  ${IMGUI_DIR}/*.h
  ${IMGUI_DIR}/*.cpp
)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")
endif()

add_library(
  IMGUI_LIB STATIC
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/GLFW/glfw3.h>
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/GLFW/glfw3native.h>
  ${IMGUI_INCLUDE}
)

target_link_libraries(
  IMGUI_LIB
  ${OPENGL_LIBRARIES}
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/lib/glfw3.Lib>
  $<$<BOOL:${UNIX}>:glfw>
  glad
)

target_include_directories(
  IMGUI_LIB PUBLIC
  ${THIRD_DIR}/imgui
  ${OPENGL_INCLUDE_DIRS}
  ${WINDOWS_INCLUDE}
  ${IMGUI_DIR}
)