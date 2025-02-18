include_directories(${DemoPrj_SOURCE_DIR}/engine/include)
include_directories(${DemoPrj_SOURCE_DIR}/engine/source)
link_directories(${DemoPrj_SOURCE_DIR}/engine/libraries)

file(GLOB ROOT_SOURCES "${DemoPrj_SOURCE_DIR}/demo/*.cpp" "${DemoPrj_SOURCE_DIR}/demo/*.h")

if (INCLUDE_GUI)
    include_directories("${DemoPrj_SOURCE_DIR}/engine/include/imgui")
    file(GLOB IMGUI_SOURCES "${DemoPrj_SOURCE_DIR}/engine/include/imgui/*.cpp")
endif()

add_executable(demo ${ROOT_SOURCES} ${IMGUI_SOURCES})

add_compile_definitions(DEBUG=$<CONFIG:Debug>)
set_target_properties(demo PROPERTIES  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${DemoPrj_SOURCE_DIR}/bin)
set_target_properties(demo PROPERTIES  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${DemoPrj_SOURCE_DIR}/bin)
target_link_libraries(demo assimp.lib glfw3.lib vulkan-1.lib)
target_link_libraries(demo source)

if (DEFINED COPY_PATH)
    file(GLOB CONTENT_SRC ${DemoPrj_SOURCE_DIR}/content/*)
    list(LENGTH CONTENT_SRC RES_LEN)
    if (RES_LEN GREATER 0)
       add_custom_command(TARGET demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${COPY_PATH}/content)
       add_custom_command(TARGET demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CONTENT_SRC} ${COPY_PATH}/content/)
    endif()
endif()