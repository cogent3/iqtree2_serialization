# iqtree2core.so (linux) or iqtree2core.dll (windows) or iqtree2core.dylib (macos) must be in the same directory as this script
import iqtree2core

print(f'IQ-TREE2 version:{iqtree2core.get_iqtree_version()}')

fai = iqtree2core.get_param("fai")
print(f'old fai parameter:{fai}')
print(f'change fai parameter to {not fai}')
iqtree2core.set_param("fai",not fai)
fai = iqtree2core.get_param("fai")
print(f'new fai:{fai}')
