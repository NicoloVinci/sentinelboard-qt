cmake_minimum_required(VERSION 3.16)

project(SentinelBoard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Charts)

qt_add_executable(SentinelBoard
    src/main.cpp
    src/mainwindow.cpp
    src/serialmanager.cpp
    src/datamodel.cpp
    src/telemetryparser.cpp
    src/idatasource.cpp
    src/SimulationDataSource.cpp

    include/mainwindow.h
    include/serialmanager.h
    include/datamodel.h
    include/telemetryparser.h
    include/telemetrysample.h
    include/idatasource.h
    include/SimulationDataSource.h

    forms/mainwindow.ui
)

target_include_directories(SentinelBoard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SentinelBoard PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)
