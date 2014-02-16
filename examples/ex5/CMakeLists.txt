cmake_minimum_required(VERSION 2.6)

project(ex5)

include_directories (../../libecam/include)

if (ecam_all)
else()
    add_subdirectory (../../libecam ./lib)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set (HEADERS player.h)
set (SOURCES player.cc ex5.cc)

add_executable(ex5 ${HEADERS} ${SOURCES})

qt5_use_modules(ex5 Widgets)

target_link_libraries (ex5 ecam)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries (ex5 pthread)
endif()
