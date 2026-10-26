byteCalc

A lightweight, minimalist, cross-platform desktop calculator built with C++17 and Qt 6.

Features
Minimalist desktop interface
Basic arithmetic operations
Decimal and negative numbers
Mathematical operator precedence
Expression parsing
Keyboard input
CMake and Ninja build system
Tech stack
C++17
Qt 6 Widgets
CMake
Ninja
Build on Linux

Install the required packages on Arch Linux:

bash
sudo pacman -S --needed base-devel gcc cmake ninja qt6-base


Configure and build:

bash
cmake -S . -B build -G Ninja
cmake --build build


Run:

bash
./build/byteCalc

Project status

Early development. Features and interface are actively being developed.

License

License not yet selected.