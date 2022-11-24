file(GLOB IMPLOT_INCLUDE CONFIGURE_DEPENDS 
  ${IMPLOT_DIR}/*.h
  ${IMPLOT_DIR}/*.cpp
)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")

  add_library(
    IMPLOT_LIB STATIC
    ${IMGUI_INCLUDE}
    ${IMPLOT_INCLUDE}
  )

  target_link_libraries(
    IMPLOT_LIB PUBLIC
    IMGUI_LIB
    glfw
    glad
  )

  target_include_directories(
    IMPLOT_LIB PUBLIC
    ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/implot
  )
  
elseif(WIN32)
  add_library(
    IMPLOT_LIB STATIC
    ${WINDOWS_INCLUDE}/GLFW/glfw3.h
    ${WINDOWS_INCLUDE}/GLFW/glfw3native.h
    ${IMGUI_INCLUDE}
    ${IMPLOT_INCLUDE}
  )

  target_link_libraries(
    IMPLOT_LIB
    IMGUI_LIB
    ${WINDOWS_INCLUDE}/lib/glfw3.Lib
    ${WINDOWS_INCLUDE}/lib/OpenGL32.Lib
    glad
  )

  target_include_directories(
    IMPLOT_LIB PUBLIC
    ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/implot
    ${WINDOWS_INCLUDE}
    ${WINDOWS_INCLUDE}
  )
endif()


