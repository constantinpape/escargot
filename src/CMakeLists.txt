# src folder

add_subdirectory(test)

if(BUILD_ESCARGOT_PYTHON)
    add_subdirectory(python)
endif()
