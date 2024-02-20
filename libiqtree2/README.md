# libiqtree2

This is a C++ library for running IQ-TREE 2 from Python. It is a wrapper around the IQ-TREE 2 library, that exports functions to Python using pybind11.

To build, run the following command (assuming the repository is cloned in `~/source/PiQTree2`):

To build without C++ unit tests:
```bash
 cd ~/source/PiQTree2  
 rm -rf build 
 mkdir build 
 cd build
 cmake -DBUILD_LIBIQTREE2_TESTS=OFF ..
 make libiqtree2        
```

To build with C++ unit tests which use Catch2 which is installed as a git submodule:

```bash
 cd ~/source/PiQTree2  
 rm -rf build 
 mkdir build 
 cd build
 cmake -DBUILD_LIBIQTREE2_TESTS=ON ..
 make libiqtree2        
```

To run C++ unit tests, run `ctest` in the build directory.

To run python tests, run `pytest` in the `libiqtree2` directory.