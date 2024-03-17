import pyIQTREE2 
import pytest

def test_pyIQTREE2_generateRandomTree():
    tree = pyIQTREE2.generateRandomTree(10)
    assert tree.count(',')+1 == 10


