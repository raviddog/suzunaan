cmake_minimum_required (VERSION 3.16)
project (thszn)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(IN_RES_DIR ./res)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

add_subdirectory(engine)
add_subdirectory(source/game)

add_executable(thszn source/main.cpp source/state.cpp)
target_include_directories(thszn PUBLIC .)
target_link_libraries(thszn engine game)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(WIN32)
        target_link_libraries(thszn -Wl,-subsystem,console)
    endif()
    if(UNIX)
        target_link_libraries(thszn ${CMAKE_DL_LIBS})
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT thszn)
    set_property(TARGET thszn PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

add_custom_command(TARGET thszn PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/res" $<TARGET_FILE_DIR:thszn>)