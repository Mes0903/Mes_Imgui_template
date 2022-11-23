include(FetchContent)
set(IMGUI_DIR ${THIRD_PARTY_DIR}/imgui)

if(WIN32)
  set(WINDOWS_INCLUDE ${THIRD_PARTY_DIR}/windows)
  add_subdirectory(windows/glad)
  
elseif(UNIX)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
  )
  
  FetchContent_MakeAvailable(glfw)
  add_subdirectory(linux/glad)
endif()

add_subdirectory(imgui)