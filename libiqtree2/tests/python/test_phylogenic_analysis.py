import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..','libs')))

import libiqtree2 as iqtree  # Adjust the import according to your actual module name

def test_phylogenic_analysis():
    alignment = "example_alignment"
    partition = "example_partition"
    initial_tree = "example_tree"
    assert iqtree.phylogenic_analysis(alignment, partition, initial_tree) == "Analysis results"
