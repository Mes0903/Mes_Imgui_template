file(GLOB IMPLOT_INCLUDE CONFIGURE_DEPENDS 
  ${IMPLOT_DIR}/*.h
  ${IMPLOT_DIR}/*.cpp
)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")
endif()

add_library(
  IMPLOT_LIB STATIC
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/GLFW/glfw3.h>
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/GLFW/glfw3native.h>
  ${IMGUI_INCLUDE}
  ${IMPLOT_INCLUDE}
)

target_link_libraries(
  IMPLOT_LIB
  IMGUI_LIB
  ${OPENGL_LIBRARIES}
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/lib/glfw3.Lib>
  $<$<BOOL:${UNIX}>:glfw>
  glad
)

target_include_directories(
  IMPLOT_LIB PUBLIC
  ${THIRD_DIR}/imgui
  ${THIRD_DIR}/implot
  ${OPENGL_INCLUDE_DIRS}
  ${WINDOWS_INCLUDE}
  ${IMPLOT_DIR}
)