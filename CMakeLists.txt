cmake_minimum_required(VERSION 3.0)
project(wblanes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-missing-braces -Wno-unused-parameter -g -pedantic")

# Search for Qt5
find_package(Qt5Core)
set(CMAKE_AUTOMOC ON)

# Search for R installation
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(R REQUIRED)

# Search for Rcpp and RInside
execute_process(COMMAND ${R_COMMAND} --vanilla --slave -e "Rcpp:::CxxFlags()" OUTPUT_VARIABLE RCPP_CXXFLAGS)
execute_process(COMMAND ${R_COMMAND} --vanilla --slave -e "Rcpp:::LdFlags()" OUTPUT_VARIABLE RCPP_LDFLAGS)
execute_process(COMMAND ${R_COMMAND} --vanilla --slave -e "RInside:::CxxFlags()" OUTPUT_VARIABLE RINSIDE_CXXFLAGS)
execute_process(COMMAND ${R_COMMAND} --vanilla --slave -e "RInside:::LdFlags()" OUTPUT_VARIABLE RINSIDE_LDFLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RCPP_CXXFLAGS} ${RINSIDE_CXXFLAGS}")

qt5_add_resources (wblanes_qrc wblanes.qrc)
add_executable(wblanes src/main.cc src/wblanes.cc ${wblanes_qrc})
qt5_use_modules(wblanes Core Gui Qml Quick Widgets)
include_directories(wblanes ${R_INCLUDE_DIR} ${Rcpp_INCLUDE_DIR})
target_link_libraries(wblanes ${R_LIBRARIES} ${RCPP_LDFLAGS} ${RINSIDE_LDFLAGS})
