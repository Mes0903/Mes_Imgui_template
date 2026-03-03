# ── glfw ──────────────────────────────────────────────────────────────────────
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# ── ImGui (GLFW + OpenGL3 backends) ───────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

add_library(IMGUI_LIB STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_vulkan.cpp
)

target_include_directories(IMGUI_LIB PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(IMGUI_LIB PUBLIC glfw ${Vulkan_LIBRARIES})

# ── ImPlot ────────────────────────────────────────────────────────────────────
set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)

add_library(IMPLOT_LIB STATIC
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${IMPLOT_DIR}/implot_demo.cpp
)

target_include_directories(IMPLOT_LIB PUBLIC ${IMPLOT_DIR})
target_link_libraries(IMPLOT_LIB PUBLIC IMGUI_LIB)
