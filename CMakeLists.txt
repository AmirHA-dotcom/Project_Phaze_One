cmake_minimum_required(VERSION 3.20)
project(Project_Phaze_One)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# اگر با Homebrew نصب کردی SDL2 رو:
include_directories(/opt/homebrew/include /opt/homebrew/include/SDL2)
link_directories(/opt/homebrew/lib)

# Manually include Eigen (for Homebrew-installed Eigen on Apple Silicon)
include_directories(/opt/homebrew/include/eigen3)

# تعریف فایل‌های پروژه
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
        #main.cpp
        Real_Diode.cpp
        Zener_Diode.cpp
        File_Handler.cpp
        plot_view.cpp
        Graphical_element.cpp
        Graphical_view.cpp
        Graphical_Wire.cpp
        Serializer.cpp
        test_serialization.cpp
        elements_graphics.cpp
)

target_link_libraries(Project_Phaze_One SDL2 SDL2_ttf SDL2_image)