import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..','libs')))
import libiqtree2 as iqtree  # Adjust the import according to your actual module name

def test_generate_random_tree():
    numtaxa = 5
    seed = 42  # Example seed for reproducibility
    tree_gen_mode = "UNIFORM"
    assert len(iqtree.generate_random_tree(numtaxa, seed, tree_gen_mode)) > 0
