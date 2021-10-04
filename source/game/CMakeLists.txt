cmake_minimum_required(VERSION 3.16)

add_library(game STATIC bullet.cpp enemy.cpp menu.cpp player.cpp stage.cpp script.cpp test3d.cpp background.cpp)
target_include_directories(game PUBLIC ../.. .. ../../engine)
set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)