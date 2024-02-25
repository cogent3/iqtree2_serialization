def iqtree_version() -> str:
    from piqtree2 import libiqtree2

    return libiqtree2.version()
