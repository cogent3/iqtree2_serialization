# CMakeLists.txt for pyIQTREE2

# derived from https://github.com/iqtree/decenttree/blob/master/pydecenttree/CMakeLists.txt

# configure a header file to pass some of the CMake settings
# to the source code
# configure_file (
#   "${PROJECT_SOURCE_DIR}/iqtree_config.h.in"
#   "${PROJECT_BINARY_DIR}/iqtree_config.h"
#   )


# Get the site-packages directory
execute_process(COMMAND ${Python3_EXECUTABLE} -c "from sysconfig import get_paths; print(get_paths()['purelib'])"
                RESULT_VARIABLE result
                OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if(result)
  message(FATAL_ERROR "Failed to find the Python site-packages directory")
endif()

if(Python3_FOUND)
  message("Python Include directory: ${Python3_INCLUDE_DIRS}")
  include_directories(${Python3_INCLUDE_DIRS})
  message("Python Libraries:         ${Python3_LIBRARIES}")
  message("Python Executable:        ${Python3_EXECUTABLE}")

  add_library(pyIQTREE2 SHARED
      pyIQTREE2.cpp 
      pyutils.cpp 
      generateRandomTree.cpp 
#      ../main/alisim.cpp 
      )

  if (APPLE)
    set(CMAKE_SHARED_LIBRARY_SUFFIX .so)
    message("Shared library suffix ${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif (APPLE)

  if (APPLE OR WIN32 )            
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -pthread")
  else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -pthread")
  endif()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lz")
  message("Open MP Linker flags:     ${OpenMP_EXE_LINKER_FLAGS}")
  message("C++ Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
  message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
  target_link_libraries(pyIQTREE2 omp)
else()
  message("Python3 Not Found")
endif()