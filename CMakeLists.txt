cmake_minimum_required(VERSION 3.20)

project(IMGUI_DEMO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

if(MSVC)
  message(STATUS "Compiler: MSVC")
  add_compile_options(/wd4819 /wd4244 /wd4267 /wd4305 "/Zc:__cplusplus" /utf-8)
  set(CMAKE_CXX_FLAGS_DEBUG   "/MDd /Zi /Od")
  set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2")
else()
  message(STATUS "Compiler: MinGW / Clang")
  add_compile_options(-Wall -Wextra)
  set(CMAKE_CXX_FLAGS_DEBUG   "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(THIRD_DIR ${PROJECT_SOURCE_DIR}/3rdparty)

add_subdirectory(${THIRD_DIR})

add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/src/imgui_demo.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${THIRD_DIR}/imgui
  ${THIRD_DIR}/implot
  ${THIRD_DIR}/glfw/include
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  IMGUI_LIB
  IMPLOT_LIB
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
  COMMENT "Copying data/ to build output"
)
