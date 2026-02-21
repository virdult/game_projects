# Create a build directory to keep source files clean
mkdir -p build && cd build

# Generate the build system
cmake ..

# Compile the projects
make

To run Tetris: ./tetris

To run TerminalGame: ./TerminalGame