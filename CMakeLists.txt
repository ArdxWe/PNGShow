cmake_minimum_required(VERSION 3.16)
project(PNGShow)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wfatal-errors -O3)

add_executable(PNGShow
               src/main.cpp
               src/Window.cpp
               src/Window.h
               src/App.cpp
               src/App.h
               src/Surface.cpp
               src/Surface.h
               src/Renderer.cpp
               src/Renderer.h
               src/Texture.cpp
               src/Texture.h
               src/Application.cpp
               src/Application.h
               src/Image.cpp
               src/Image.h
               src/Ttf.cpp
               src/Ttf.h
               src/Font.cpp
               src/Font.h)

TARGET_LINK_LIBRARIES(PNGShow
                      pthread
                      SDL2
                      SDL2_ttf
                      SDL2_image)