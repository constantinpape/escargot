set(PYTHON_VERSION "" CACHE STRING "Python version to use for compiling the library")

set(Python_ADDITIONAL_VERSIONS 3.4 3.5 3.6)
find_package(PythonLibs ${PYTHON_VERSION} REQUIRED)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # Enable C++11 mode on C++ / Clang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

  # Enable link time optimization and set the default symbol
  # visibility to hidden (very important to obtain small binaries)
  if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    # Default symbol visibility
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

    # Check for Link Time Optimization support
    #CHECK_CXX_COMPILER_FLAG("-flto" HAS_LTO_FLAG)
    #if (HAS_LTO_FLAG)
    #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
    #endif()
  endif()
endif()


# Create the binding library
add_library(escargot_module SHARED
    escargot_module.cxx
    # ... extra files go here ...
    #algorithm/algorithm.cxx
)

set_target_properties(escargot_module PROPERTIES PREFIX "")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/python/escargot)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test_python/)

ADD_CUSTOM_COMMAND(
        TARGET escargot_module
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different $<TARGET_FILE:escargot_module> ${CMAKE_BINARY_DIR}/python/escargot/
        COMMENT "Copying target to temporary module directory")


add_custom_target(copy_python__init__ ALL#POST_BUILD
    COMMAND cmake -E copy_if_different 
    ${CMAKE_SOURCE_DIR}/src/python/module/__init__.py 
    ${CMAKE_BINARY_DIR}/python/escargot/__init__.py)



#add_custom_target(copy_python_test ALL#POST_BUILD
#    COMMAND cmake -E copy_if_different 
#    ${CMAKE_SOURCE_DIR}/src/python/test/test_python.py 
#    ${CMAKE_BINARY_DIR}/test_python/test_python.py)

