from piqtree2 import iqtree_version
import re
import sys

def test_getIqTreeVersion():
    version = iqtree_version()
    assert isinstance(version, str) and len(version) > 0
    assert version.startswith("IQ-TREE")
    assert version.endswith("COVID-edition")

    match = re.search(r'\d+\.\d+\.\d+', version)
    if match:
        assert match.group() == "2.2.5"
    else:
        assert False, "unable to find version number in string: " + version