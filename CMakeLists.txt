# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.20)

# Name the project
project(Project_Phaze_One)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SDL2_DEV_ARCH_DIR "E:/newest_SDL/SDL2-2.32.8/x86_64-w64-mingw32")

include_directories(
        ${SDL2_DEV_ARCH_DIR}/include
        ${SDL2_DEV_ARCH_DIR}/include/SDL2
)

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
        graphical_view.h
        graphical_view.cpp
        elements_graphics.cpp
        graphical_element.cpp
        graphical_element.h
        Graphical_Wire.cpp
        Graphical_Wire.h
)

target_link_libraries(Project_Phaze_One
        -L${SDL2_DEV_ARCH_DIR}/lib

        -lmingw32
        #-lconio

        -lSDL2main
        -lSDL2

        # SDL2 Extension libraries (ensure you have downloaded these development libs for your architecture
        # and placed them within E:/newest_SDL/SDL2-2.32.8/x86_64-w64-mingw32/lib)
#        -lSDL2_image
        -lSDL2_ttf
#        -lSDL2_mixer
)