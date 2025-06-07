cmake_minimum_required(VERSION 3.29)
project(Project_Phaze_One)

set(CMAKE_CXX_STANDARD 20)

add_executable(Project_Phaze_One
        main.cpp
        Node.cpp
        Node.h
        Element.cpp
        Element.h
        Resistor.cpp
        Resistor.h
        Capacitor.cpp
        Capacitor.h
        Inductor.cpp
        Inductor.h
        Voltage_Source.cpp
        Voltage_Source.h
        Circuit.cpp
        Circuit.h
        Libraries.h
        Algorithems.cpp
        Algorithems.h
        View.cpp
        View.h
)