import piqtree2


def test_getIqTreeVersion():
     
    version = piqtree2.get_iqtree_version()
    assert isinstance(version, str) and len(version) > 0
    assert version.startswith("IQ-TREE version")
    assert version.endswith("COVID-edition")
    assert version[16:-14].strip() == "2.5.5"
