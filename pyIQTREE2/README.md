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
