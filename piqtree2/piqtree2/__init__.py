from .version import get_iqtree_version
from .params import set_params, get_params, get_param

import os
import sys

# This will navigate up to '/iqtree2/piqtree2/' and then into the 'libs' directory
libs_dir = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'libs')

if libs_dir not in sys.path:
    sys.path.insert(0, libs_dir)

# Now we can import the shared library in libs
import iqtree2core

