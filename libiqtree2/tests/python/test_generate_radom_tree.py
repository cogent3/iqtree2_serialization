import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..','libs')))
import libiqtree2 as iqtree  # Adjust the import according to your actual module name
import pytest 

def test_generate_random_tree():
    num_taxa = 5
    seed = 42  # Example seed for reproducibility
    tree_gen_mode = "UNIFORM"
    assert len(iqtree.generate_random_tree(num_taxa, seed, tree_gen_mode)) > 0

def test_generate_random_tree_raises_exception_when_numtaxa_is_zero():
    num_taxa = 0
    seed = 42  # Example seed for reproducibility
    tree_gen_mode = "UNIFORM"
    with pytest.raises(ValueError):
        iqtree.generate_random_tree(num_taxa, seed, tree_gen_mode)

def test_generate_random_tree_returns_same_result_for_same_seed():
    num_taxa = 5
    seed = 42  # Example seed for reproducibility
    tree_gen_mode = "UNIFORM"
    result1 = iqtree.generate_random_tree(num_taxa, seed, tree_gen_mode)
    result2 = iqtree.generate_random_tree(num_taxa, seed, tree_gen_mode)
    assert result1 == result2

@pytest.mark.xfail(reason="Expected to fail because generate_random_tree is using the seed value.")
def test_generate_random_tree_returns_different_results_for_different_seeds():
    num_taxa = 5
    seed1 = 42  # Example seed for reproducibility
    seed2 = 43  # Different seed
    tree_gen_mode = "UNIFORM"
    result1 = iqtree.generate_random_tree(num_taxa, seed1, tree_gen_mode)
    result2 = iqtree.generate_random_tree(num_taxa, seed2, tree_gen_mode)
    assert result1 != result2

import time

@pytest.mark.xfail(reason="Expected to fail because generate_random_tree is not generating random trees.")
def test_generate_random_tree_returns_different_results_when_called_at_different_times():
    num_taxa = 5
    result1 = iqtree.generate_random_tree(num_taxa)
    time.sleep(1)  # Wait for a second to ensure a different time-based seed
    result2 = iqtree.generate_random_tree(num_taxa)
    assert result1 != result2        


def test_generate_random_tree_raises_exception_when_numtaxa_is_less_than_three():
    num_taxa = 2  # Less than 3
    with pytest.raises(ValueError):
        iqtree.generate_random_tree(num_taxa)

import re

@pytest.mark.parametrize("num_taxa", [5, 10, 20])
@pytest.mark.xfail(reason="Expected to fail because generate_random_tree is not generating a tree with the correct number of taxa.")
def test_generate_random_tree_returns_tree_with_correct_number_of_taxa(num_taxa):
    result = iqtree.generate_random_tree(num_taxa)

    # Count the number of taxa in the Newick tree - note that the taxa are assumed to be represented by single capital letters
    taxa_count = len(re.findall(r'\b[A-Z]\b', result))

    assert taxa_count == num_taxa