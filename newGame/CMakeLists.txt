cmake_minimum_required(VERSION 3.10)
project(TerminalGame)

set(CMAKE_CXX_STANDARD 17)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add all the cpp files
add_executable(TerminalGame
    src/main.cpp
    src/world.cpp
    src/character.cpp
    src/mobs.cpp
    src/projectiles.cpp
)

# Link SDL2 and include directories
target_include_directories(TerminalGame PRIVATE include ${SDL2_INCLUDE_DIRS})
target_link_libraries(TerminalGame PRIVATE ${SDL2_LIBRARIES})