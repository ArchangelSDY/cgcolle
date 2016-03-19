cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0042 NEW)   # Set MACOSX_RPATH default to ON
cmake_policy(SET CMP0048 NEW)   # Set PROJECT_VERSION properly

project (cgcolle VERSION 0.1.0)

set(Boost_USE_STATIC_LIBS TRUE)

find_package(Boost COMPONENTS date_time filesystem locale program_options regex system REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS ${OpenCV_INCLUDE_DIRS})

if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()

add_subdirectory(lib)
add_subdirectory(encoder)
add_subdirectory(viewer)