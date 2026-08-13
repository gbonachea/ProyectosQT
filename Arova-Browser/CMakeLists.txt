cmake_minimum_required(VERSION 3.16)
project(Arova VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network WebEngineWidgets)

add_definitions(-DPKGDATADIR="${CMAKE_INSTALL_PREFIX}/share/arova")

add_subdirectory(src)
