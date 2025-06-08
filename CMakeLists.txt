# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.20)

# Name the project
project(Project_Phaze_One)

# Set the C++ standard to C++17 for better compatibility
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable and list ONLY the .cpp source files
add_executable(Project_Phaze_One
        Node.cpp
        Element.cpp
        Resistor.cpp
        Capacitor.cpp
        Inductor.cpp
        Voltage_Source.cpp
        Current_Source.cpp
        CCCS.cpp
        CCVS.cpp
        VCCS.cpp
        VCVS.cpp
        Circuit.cpp
        Algorithems.cpp
        View.cpp
        Errors.cpp
        Controller.cpp
        main.cpp
        Real_Diode.cpp
        Zener_Diode.cpp
        File_Handler.cpp
        File_Handler.h
)