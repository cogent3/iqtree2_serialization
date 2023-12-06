def get_iqtree_version() -> str:
    from piqtree2 import iqtree2core

    return iqtree2core.get_iqtree_version()
