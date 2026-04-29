cmake_minimum_required(VERSION 3.21.1)
project(TuoProgetto LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Charts)

add_executable(TuoProgetto
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/alarmmanager.cpp
    ./src/csvlogger.cpp
    ./src/datamodel.cpp
    ./src/serialmanager.cpp
    ./src/telemetryparser.cpp
    ./forms/mainwindow.ui
    ./include/alarmmanager.h
    ./include/csvlogger.h
    ./include/datamodel.h
    ./include/mainwindow.h
    ./include/serialmanager.h
    ./include/telemetryparser.h
)

target_link_libraries(TuoProgetto PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)