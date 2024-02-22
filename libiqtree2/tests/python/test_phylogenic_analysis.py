import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..','libs')))

import libiqtree2 as iqtree  # Adjust the import according to your actual module name
from unittest.mock import Mock

def test_phylogenic_analysis():
    alignment = "example_alignment"
    partition = "example_partition"
    model = "example_model"
    initial_tree = "example_tree"

    # create a mock object for the log callback
    mock_log_callback = Mock()

    assert iqtree.phylogenic_analysis(alignment, partition, model, initial_tree,log_callback=mock_log_callback) == "Analysis results"
    mock_log_callback.assert_called()
    mock_log_callback.assert_any_call("sample log message\n")
    mock_log_callback.call_count == 1