cmake_minimum_required(VERSION 3.21)

project(byteCalc
        VERSION 0.1
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDAR_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(byteCalc
    src/main.cpp
    src/calcengine.cpp
    src/calcengine.h
)

target_link_libraries(byteCalc
    PRIVATE Qt6::Widgets
)

install(TARGETS byteCalc
    RUNTIME DESTINATION bin
)