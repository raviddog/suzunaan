cmake_minimum_required(VERSION 3.16)

add_library(imgui STATIC  imgui_impl_glfw.cpp imgui_impl_opengl3.cpp imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp)