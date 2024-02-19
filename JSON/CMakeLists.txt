##############################################################
# Build iqtree2core shared library
##############################################################



find_package(nlohmann_json 3.11 REQUIRED)
find_package(PythonLibs REQUIRED)

include(FetchContent)

# Fetch pybind11
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG v2.11.1  # Last stable release as of 2021-07-11
)
FetchContent_MakeAvailable(pybind11)

# Define the path to the piqtree2 sources
set(IQTREE2CORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)

# Collect the header files for the iqtree2core library
set(IQTREE2CORE_HEADERS 
    "${IQTREE2CORE_SOURCE_DIR}/version.h"
    "${IQTREE2CORE_SOURCE_DIR}/params.h"
    "${IQTREE2CORE_SOURCE_DIR}/generate_random_tree.h"
    "${PROJECT_BINARY_DIR}/iqtree_config.h"
    "${IQTREE2CORE_SOURCE_DIR}/iqtree2core_init.h"
    )

# Collect the source files for the piqtree2 library
set (IQTREE2CORE_SOURCES 
    "${IQTREE2CORE_SOURCE_DIR}/version.cpp"
    "${IQTREE2CORE_SOURCE_DIR}/params.cpp"
    "${IQTREE2CORE_SOURCE_DIR}/generate_random_tree.cpp"
    "${IQTREE2CORE_SOURCE_DIR}/iqtree2core_init.cpp"
    "${PROJECT_SOURCE_DIR}/utils/tools.cpp"
    )

set (IQTREE2CORE_BINDINGS 
    "${IQTREE2CORE_SOURCE_DIR}/version_bindings.cpp"
    "${IQTREE2CORE_SOURCE_DIR}/params_bindings.cpp"
    "${IQTREE2CORE_SOURCE_DIR}/generate_random_tree_bindings.cpp"
    )

# Add a shared library target
add_library(iqtree2core MODULE 
    ${IQTREE2CORE_SOURCES} 
    ${IQTREE2CORE_BINDINGS}
    )

target_include_directories(iqtree2core PRIVATE
    ${IQTREE2CORE_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}  # Add this line for iqtree_config.h
    )

target_include_directories(iqtree2core PRIVATE
    ${nlohmann_json_INCLUDE_DIRS}
    )

target_link_libraries(iqtree2core PRIVATE pybind11::module)

if(Backtrace_FOUND)
  target_link_libraries(iqtree2core PRIVATE ${Backtrace_LIBRARY})
endif(Backtrace_FOUND)

if (USE_BOOSTER)
    target_link_libraries(iqtree2core PRIVATE booster)
endif()

if (NOT IQTREE_FLAGS MATCHES "single")
    if(CLANG AND WIN32)
        if (BINARY32)
            target_link_libraries(iqtree2core PRIVATE ${PROJECT_SOURCE_DIR}/lib32/libiomp5md.dll)
        else()
            target_link_libraries(iqtree2core PRIVATE ${PROJECT_SOURCE_DIR}/lib/libiomp5md.dll)
        endif()
    endif()
endif()

if(ZLIB_FOUND)
  target_link_libraries(iqtree2core PRIVATE ${ZLIB_LIBRARIES})
else(ZLIB_FOUND)
  target_link_libraries(iqtree2core PRIVATE zlibstatic)
endif(ZLIB_FOUND)


# Link the shared library to the required dependencies
# target_link_libraries(iqtree2core PRIVATE
#    pybind11::module utils model alignment tree simulator terrace yaml-cpp phyloYAML main ${PLATFORM_LIB} ${STD_LIB} ${THREAD_LIB} ${ATOMIC_LIB})

# basic linking librararies
target_link_libraries(iqtree2core PRIVATE pll ncl lbfgsb sprng vectorclass gsl yaml-cpp nclextra whtest model phyloYAML alignment utils simulator tree pda terrace main ${PLATFORM_LIB} ${STD_LIB} ${THREAD_LIB} ${ATOMIC_LIB})

if (USE_TERRAPHAST)
   target_link_libraries(iqtree2core PRIVATE terraphast terracetphast)
endif()

if (USE_LSD2)
    target_link_libraries(iqtree2core PRIVATE lsd2)
endif()

if (NOT IQTREE_FLAGS MATCHES "nosse")
    target_link_libraries(iqtree2core PRIVATE kernelsse)
endif()

# MPI libraries
if (IQTREE_FLAGS MATCHES "mpi")
    if (NOT CMAKE_CXX_COMPILER MATCHES "mpi")
        target_link_libraries(iqtree2core PRIVATE ${MPI_CXX_LIBRARIES})
    endif()
endif()

# SSE, AVX etc. libraries
if (NOT BINARY32 AND NOT IQTREE_FLAGS MATCHES "novx")
    target_link_libraries(iqtree2core PRIVATE pllavx kernelavx kernelfma)
    if (IQTREE_FLAGS MATCHES "KNL")
        target_link_libraries(iqtree2core PRIVATE kernelavx512)
    endif()
endif()

if (NOT IQTREE_FLAGS MATCHES "avx" AND NOT IQTREE_FLAGS MATCHES "fma")
    if (NOT IQTREE_FLAGS MATCHES "nosse")
        set_target_properties(iqtree2core PROPERTIES COMPILE_FLAGS "${SSE_FLAGS}")
    endif()
endif()

# Define properties for the Python module
set_target_properties(iqtree2core PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"  # this is usually "" on all systems
    SUFFIX "${PYTHON_MODULE_EXTENSION}"  # this is .pyd on Windows, .so on Linux and MacOS
)

# Specify the public headers for installation
set_target_properties(iqtree2core PROPERTIES
    PUBLIC_HEADER "${IQTREE2CORE_HEADERS}"
)

# OS-specific library output names and suffixes
if (WIN32)
    set_target_properties(iqtree2core PROPERTIES
        OUTPUT_NAME "iqtree2core"
        SUFFIX ".dll"
    )
elseif (APPLE)
    set_target_properties(iqtree2core PROPERTIES
        OUTPUT_NAME "iqtree2core"
        SUFFIX ".dylib"
    )
else()
    set_target_properties(iqtree2core PROPERTIES
        OUTPUT_NAME "iqtree2core"
        SUFFIX ".so"
    )
endif()

# Specify installation instructions for the shared library
install(TARGETS iqtree2core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # For DLLs on Windows
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
