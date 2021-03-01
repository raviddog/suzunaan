cmake_minimum_required (VERSION 3.16)
project (thszn)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(IN_RES_DIR ./res)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./bin)

add_library(engine STATIC engine/engine_gl.cpp engine/engine.cpp engine/glad.c engine/stb_image.cpp engine/debug.cpp)
target_include_directories(engine PUBLIC ./engine)
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
        target_link_libraries(engine -lSDL2main -lSDL2)
    endif()
    if(WIN32)
        target_link_libraries(engine -lmingw32 -lSDL2main -lSDL2)
    endif()
    #cool way
    #find_package(ASSIMP CONFIG REQUIRED)
    #if(ASSIMP_FOUND)
    #    include_directories(${ASSIMP_INCLUDE_DIR})
    #endif()

    #target_link_libraries(engine -lmingw32 -lSDL2main -lSDL2 ${ASSIMP_LIBRARY})
    #target_link_libraries(engine -lmingw32 -lSDL2main -lSDL2 -lassimp-vc142-mt.lib)
endif()

add_library(game STATIC source/game/bullet.cpp source/game/enemy.cpp source/game/menu.cpp source/game/player.cpp source/game/stage.cpp source/game/script.cpp source/game/test3d.cpp source/game/background.cpp)
target_include_directories(game PUBLIC . ./engine ./source)
set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(thszn source/main.cpp source/state.cpp)
target_include_directories(thszn PUBLIC .)
target_link_libraries(thszn engine game)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(WIN32)
        target_link_libraries(thszn -Wl,-subsystem,console)
    endif()
    if(UNIX)
        target_link_libraries(thszn -Wl,-subsystem -ldl)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT thszn)
    set_property(TARGET thszn PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

#target_link_libraries(thszn -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer)

add_custom_command(TARGET thszn PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/res" $<TARGET_FILE_DIR:thszn>)