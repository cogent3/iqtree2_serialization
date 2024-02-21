import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..','libs')))
import libiqtree2 as iqtree  # Adjust the import according to your actual module name

def test_calculate_RF_distance():
    tree1 = "(A,B);"
    tree2 = "(B,A);"
    assert iqtree.calculate_RF_distance(tree1, tree2) == 0  # Assuming 0 is the expected distance
