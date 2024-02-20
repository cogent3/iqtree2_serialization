import libiqtree2 as iqtree  # Adjust the import according to your actual module name

def test_generate_random_tree():
    numtaxa = 5
    seed = 42  # Example seed for reproducibility
    branch_length_mode = "uniform"
    assert len(iqtree.generate_random_tree(numtaxa, seed, branch_length_mode)) > 0
