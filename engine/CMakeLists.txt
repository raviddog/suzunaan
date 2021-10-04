cmake_minimum_required(VERSION 3.16)

add_library(engine STATIC engine_gl.cpp engine.cpp glad.c debug.cpp util.cpp libs/single_file.cpp)
target_include_directories(engine PUBLIC ./)
set_property(TARGET engine PROPERTY CXX_STANDARD 17)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_include_directories(engine PUBLIC    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/um"
                                                "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/shared"
                                                "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0/um/x64")
    target_link_libraries(engine winmm.lib)
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(UNIX)
        target_link_libraries(engine -lglfw)
    endif()
    if(WIN32)
        target_link_libraries(engine -lmingw32 -lglfw3)
    endif()
endif()

add_subdirectory(libs/imgui)
target_link_libraries(engine imgui)