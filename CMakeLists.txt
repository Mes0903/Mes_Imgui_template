# MesImgui/CMakeLists.txt

cmake_minimum_required(VERSION 3.8)
 
project(MesImgui VERSION 0.1.0)
 
set(CMAKE_CXX_STANDARD 11)
 
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${THIRD_PARTY_DIR})
add_subdirectory(${THIRD_PARTY_DIR})

set(LIB_PATH ${PROJECT_SOURCE_DIR}/lib)
file(GLOB_RECURSE LIB_FILES
	"${LIB_PATH}/*.lib"
)
link_directories(${LIB_PATH}/lib)

aux_source_directory(./src DIR_SRCS)
add_executable(${CMAKE_PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(MesImgui IMGUI)
target_link_libraries(MesImgui ${LIB_FILES})