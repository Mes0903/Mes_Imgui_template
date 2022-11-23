file(GLOB IMGUI_INCLUDE CONFIGURE_DEPENDS 
  ${IMGUI_DIR}/*.h
  ${IMGUI_DIR}/*.cpp
)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")

  add_library(
    IMGUI_LIB STATIC
    ${IMGUI_INCLUDE}
  )

  target_link_libraries(
    IMGUI_LIB PUBLIC
    glfw
    glad
  )

  target_include_directories(
    IMGUI_LIB PUBLIC
    imgui
  )
  
elseif(WIN32)
  add_library(
    IMGUI_LIB STATIC
    ${WINDOWS_INCLUDE}/GLFW/glfw3.h
    ${WINDOWS_INCLUDE}/GLFW/glfw3native.h
    ${IMGUI_INCLUDE}
  )

  target_link_libraries(
    IMGUI_LIB
    ${WINDOWS_INCLUDE}/lib/glfw3.Lib
    ${WINDOWS_INCLUDE}/lib/OpenGL32.Lib
    glad
  )

  target_include_directories(
    IMGUI_LIB PUBLIC
    imgui
    ${WINDOWS_INCLUDE}
    ${WINDOWS_INCLUDE}
  )
endif()


