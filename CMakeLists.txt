cmake_minimum_required(VERSION 3.0)
project(wblanes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -g -pedantic")

find_package(Qt5Core)
set(CMAKE_AUTOMOC ON)

add_executable(wblanes src/main.cc)
qt5_use_modules(wblanes Core Qml Quick)
