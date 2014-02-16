cmake_minimum_required (VERSION 2.6)

set (ecam_all true)

set (PROJECT ecam)

project (${PROJECT})

add_subdirectory (./libecam lib)
add_subdirectory (./examples/ex1 ex1)
add_subdirectory (./examples/ex2 ex2)
add_subdirectory (./examples/ex3 ex3)
add_subdirectory (./examples/ex5 ex5)
