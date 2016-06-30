cmake_minimum_required(VERSION 2.8)

project(escargot)

include(CheckCXXCompilerFlag)

# flag for pybindings
option(BUILD_ESCARGOT_PYTHON OFF)


# check for c++ 11 support
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)
    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    elseif (HAS_CPP11_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        message(FATAL_ERROR "Unsupported compiler -- escargot  requires C++11 support!")
    endif()
endif()

# lib rt
find_library(RT rt)
set(LINK_RT true)

# Set a default build configuration if none is specified. 'MinSizeRel' produces the smallest binaries
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'MinSizeRel' as none was specified.")
    set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

# externals
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/externals/marray/include" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/externals/pybind11/include" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/externals/googletest/googletest/include" )

# tests
enable_testing()

# global headers
file(GLOB_RECURSE headers include/*.hxx)
file(GLOB_RECURSE headers ${CMAKE_INSTALL_PREFIX}/include/*.hxx)
include_directories(include)

# src folder 
add_subdirectory(src)
