cmake_minimum_required(VERSION 3.29)
project(Project_Phaze_One)

set(CMAKE_CXX_STANDARD 20)

# Collect all .cpp files in the project root
file(GLOB PROJECT_SOURCES
        "${CMAKE_SOURCE_DIR}/*.cpp"
)
add_executable(Project_Phaze_One
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
        Current_Source.cpp
        Current_Source.h
        CCCS.cpp
        CCCS.h
        CCVS.cpp
        CCVS.h
        VCCS.cpp
        VCCS.h
        VCVS.cpp
        VCVS.h
        Circuit.cpp
        Circuit.h
        Libraries.h
        Algorithems.cpp
        Algorithems.h
        View.cpp
        View.h
        Errors.cpp
        Errors.h
        Controller.cpp
        Controller.h
        main.cpp
        Real_Diode.h
        Real_Diode.cpp
        Zener_Diode.h
        Zener_Diode.cpp
)
