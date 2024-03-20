include(FetchContent)
set(IMGUI_DIR ${THIRD_DIR}/imgui)
set(IMPLOT_DIR ${THIRD_DIR}/implot)
set(IMFD_DIR ${THIRD_DIR}/ImGuiFileDialog)

add_subdirectory(glfw-3.4)

if(WIN32)
  add_subdirectory(windows/glad)
else()
  add_subdirectory(linux/glad)
endif()

add_subdirectory(imgui)
add_subdirectory(implot)