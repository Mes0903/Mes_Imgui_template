# MesOpengl/include/CMakeLists.txt

file(GLOB P_INCLUDE CONFIGURE_DEPENDS 
    ${THIRD_PARTY_DIR}/glab/*.h
    ${THIRD_PARTY_DIR}/GLFW/*.h
    ${THIRD_PARTY_DIR}/imgui/*.h
    ${THIRD_PARTY_DIR}/KHR/*.h
)

file(GLOB P_SOURCE CONFIGURE_DEPENDS 
    ${THIRD_PARTY_DIR}/glad/glad.c
    ${THIRD_PARTY_DIR}/imgui/*.cpp
)

add_library(IMGUI STATIC 
    ${P_INCLUDE} 
    ${P_SOURCE}
)

target_include_directories(IMGUI PRIVATE include
    ${THIRD_PARTY_DIR}/glad
    ${THIRD_PARTY_DIR}/GLFW
    ${THIRD_PARTY_DIR}/KHR
    ${THIRD_PARTY_DIR}/imgui
)

link_directories(${LIB_PATH}/lib)
target_link_libraries(IMGUI ${LIB_FILES})