import os
import sys

# from .params import get_param, get_params, set_params
from .version import iqtree_version

# This will navigate up to '../piqtree2/' and then into the 'libs' directory
libs_dir = os.path.join(os.path.dirname(os.path.dirname(__file__)), "libs")

if libs_dir not in sys.path:
    sys.path.insert(0, libs_dir)

# Now we can import the shared library in libs
import libiqtree2
