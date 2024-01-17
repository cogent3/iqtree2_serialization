cmake_minimum_required(VERSION 3.12)
project(example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -fPIC")

# Find Python interpreter, libraries, and headers
find_package(PythonLibs REQUIRED)

# Include FetchContent module
include(FetchContent)

# Fetch pybind11
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG v2.11.1  # Last stable release as of 2021-07-11
)
FetchContent_MakeAvailable(pybind11)

# Add the executable
add_library(example MODULE example.cpp)

# Link against the Python libraries and the pybind11 libraries
target_link_libraries(example PRIVATE pybind11::module ${PYTHON_LIBRARIES})

# Include the Python headers and the pybind11 headers
target_include_directories(example PRIVATE ${PYTHON_INCLUDE_DIRS})

# Set the output name to include the Python extension suffix
set_target_properties(example PROPERTIES PREFIX ""
                                        SUFFIX ".so")