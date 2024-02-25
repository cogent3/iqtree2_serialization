import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..','libs')))

import libiqtree2 as iqtree

def test_version():
    assert "2.2.5" in iqtree.version()