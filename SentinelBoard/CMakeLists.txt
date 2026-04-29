cmake_minimum_required(VERSION 3.21.1)
project(TuoProgetto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/forms
)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Charts)

qt_add_executable(TuoProgetto
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

target_include_directories(TuoProgetto PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TuoProgetto PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)