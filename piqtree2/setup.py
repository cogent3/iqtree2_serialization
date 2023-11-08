import os
import subprocess
import sys

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# Define the root directory of your Python package (e.g., where setup.py is located)
package_root_dir = os.path.abspath(os.path.dirname(__file__))
libs_dir = os.path.join(package_root_dir, 'libs')

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            for ext in self.extensions:
                self.build_extension(ext)
        except subprocess.CalledProcessError as e:
            print(e.stdout.decode())
            print(e.stderr.decode())
            raise RuntimeError("CMake failed!")

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + libs_dir,
            "-DPYTHON_EXECUTABLE=" + sys.executable,
            "-DIQTREE_FLAGS=static",
        ]

        cfg = "Debug" if self.debug else "Release"
        build_args = ["--config", cfg]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.run(
            ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp, check=True
        )
        subprocess.run(
            ["cmake", "--build", ".", "--target", "iqtree2core"] + build_args,
            cwd=self.build_temp,
            check=True,
        )

        # Since we removed the call to the `super().build_extension(ext)`,
        # there's no need for any additional build steps provided by setuptools.


setup(
    name="piqtree2",
    version="0.1",
    author="Richard Morris",
    author_email="richard.morris@anu.edu.au",
    description="A Python wrapper for the iqtree2core library",
    long_description="",
    packages=["piqtree2"],
    ext_modules=[CMakeExtension("piqtree2", sourcedir="..")],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
    install_requires=[
        "cogent3",
    ],
    extras_require={
        "dev": [
            "black==23.9.1",
            "isort==5.12.0",
            "pytest",
        ],
    },
)
