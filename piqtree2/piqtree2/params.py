from typing import Dict, Any

# Global dictionary to store parameters
params: Dict[str, Any] = {}

def set_params(json_str: Dict[str, Any]) -> None:
    from . import iqtree2core
    iqtree2core.set_params(json_str)
    # Update the local params dictionary if needed
    params.update(json_str)

def get_params() -> Dict[str, Any]:
    from . import iqtree2core
    return iqtree2core.get_params()

def get_param(name: str) -> Any:
    from . import iqtree2core
    return iqtree2core.get_param(name)
