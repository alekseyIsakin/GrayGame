cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(DemoPrj)

set(INCLUDE_GUI 1)
add_compile_definitions(FORCE_VSYNC)

set(COPY_PATH ${DemoPrj_SOURCE_DIR}/bin)

if(INCLUDE_GUI)
    add_compile_definitions(INCLUDE_GUI)
endif()

add_subdirectory(engine)
add_subdirectory(demo)
