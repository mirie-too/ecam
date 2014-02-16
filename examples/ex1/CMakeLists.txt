cmake_minimum_required (VERSION 2.6)

set (PROJECT ex1)

project (${PROJECT})

include_directories (../../libecam/include)

if (ecam_all)
else()
    add_subdirectory (../../libecam ./lib)
endif()

set (HEADERS
		)

set (SOURCES 
		ex1.cc)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries (${PROJECT} ecam)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries (${PROJECT} pthread)
endif()

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
