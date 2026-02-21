cmake_minimum_required(VERSION 3.10)
project(TetrisGame)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

include_directories(include)

find_package(Threads REQUIRED)

set(SOURCES 
    src/main.cpp 
    src/game.cpp
)

add_executable(tetris ${SOURCES})

target_link_libraries(tetris PRIVATE Threads::Threads)