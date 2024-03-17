# pyIQTREE2

This directory contains the source code for the `pyIQTREE2` project. `pyIQTREE2` is a Python interface for the IQ-TREE software.

## Requirements

- Python 3.10
- CMake
- A C++ compiler with OpenMP support

## Installation

To install the `pyIQTREE2` library, navigate to the pyIQTREE2 directory (containing the `setup.py` file) and run the following command:

```bash
pip install -e .
```

This will compile the C++ code and install the pyIQTREE2 Python module.

## Usage
You can import the pyIQTREE2 module in your Python scripts using the following line of code:

```python
from pyIQTREE2 import generateRandomTree

print(generateRandomTree(10))
```

To print out the result of a randomtree containing 10 items

## Running Tests

To run the unit tests, you need to have pytest installed. If it's not installed, you can install it with the following command:

```bash
pip install pytest
```

Then, you can run the tests with the following command:

```bash
pytest
``` 

The results of the tests will be printed to the console.

```bash
(venv) ➜  pyIQTREE2 git:(pyIQTREE2) pytest
=========================================================================== test session starts ===========================================================================
platform linux -- Python 3.11.2, pytest-8.0.1, pluggy-1.4.0
rootdir: /iqtree2/pyIQTREE2
plugins: anyio-4.3.0
collected 1 item                                                                                                                                                          

test_pyIQTREE2.py F                                                                                                                                                 [100%]

================================================================================ FAILURES =================================================================================
____________________________________________________________________ test_pyIQTREE2_generateRandomTree ____________________________________________________________________

    def test_pyIQTREE2_generateRandomTree():
        tree = pyIQTREE2.generateRandomTree(10)
>       assert tree.count(',')+1 == 10
E       AssertionError: assert (2 + 1) == 10
E        +  where 2 = <built-in method count of str object at 0x7f7072b563d0>(',')
E        +    where <built-in method count of str object at 0x7f7072b563d0> = '((A:0.1,B:0.2):0.3,C:0.4);'.count

test_pyIQTREE2.py:6: AssertionError
========================================================================= short test summary info =========================================================================
FAILED test_pyIQTREE2.py::test_pyIQTREE2_generateRandomTree - AssertionError: assert (2 + 1) == 10
============================================================================ 1 failed in 0.25s ============================================================================
```