from   distutils.core import setup, Extension

dirs   = [".."]
flags  = ['-std=c++11']
link_flags = []

#
#This is how I hacked OpenMP support on OS X 12.4
#Outside setup.py.. run these:
#
#>  export CFLAGS="-Xpreprocessor -fopenmp $CFLAGS"
#>  export CXXFLAGS="-Xpreprocessor -fopenmp $CXXFLAGS"
#b
#(you need to set both CFLAGS and CXXFLAGS!  Don't know why)
#...but I get lots of repeated warnings out of signal.h when I build.
#
import os
if ('CXXFLAGS' in os.environ):
    if (os.environ['CXXFLAGS'].find('-Xpreprocessor')!=-1):
        link_flags.append('-lomp')
        dirs.append('/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include')

module1 = Extension("pyIQTREE2",
                    sources = ["pyIQTREE2.cpp",  
                               "generateRandomTree.cpp",
                               "../main/alisim.cpp"],
                    include_dirs = dirs,
                    extra_compile_args = flags,
                    extra_link_args= link_flags,
                    language = "c++")


def main():
    setup(name="pyIQTREE2",
          version="0.1.0",
          description="Python interface for IQTREE2",
          author="Richard Morris (2024)",
          author_email="richard.morris@anu.edu.au",
          ext_modules=[module1],
          setup_requires = [],
          tests_require = ['pytest'],
          install_requires = [])

if __name__ == "__main__":
    main()