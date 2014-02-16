cmake_minimum_required (VERSION 2.6)

set (PROJECT ecam)

project (${PROJECT})

include_directories (include)

set (HEADERS
    include/ecam/camera.h
    include/ecam/capabilities.h
    include/ecam/config.h
    include/ecam/decoder.h
    include/ecam/frame.h
    include/ecam/localcamera.h
    include/ecam/processor.h
    include/ecam/remotecamera.h
    include/ecam/thread.h
)

set (SOURCES
    src/camera.cc
    src/capabilities.cc
    src/config.cc
    src/frame.cc
    src/localcamera.cc
    src/thread.cc
)

add_library (${PROJECT} ${HEADERS} ${SOURCES})
