# src folder

# FIXME won't link without python libraries, fix when pythnonbindings done properly
#add_subdirectory(test)

if(BUILD_ESCARGOT_PYTHON)
    add_subdirectory(python)
endif()
