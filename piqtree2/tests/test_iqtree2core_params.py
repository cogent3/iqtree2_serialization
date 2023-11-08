import pytest

import piqtree2


def test_param_manipulation():
    initial_params = piqtree2.get_params()
    # test initial_params contains a key 'fai'
    assert (
        "fai" in initial_params
    ), "get_params: Parameter 'fai' not found in initial parameters."

    original_value = initial_params["fai"]
    piqtree2.set_params({"fai": not original_value})
    updated_value = piqtree2.get_param("fai")
    assert (
        updated_value != original_value
    ), "set_params: Parameter 'fai' did not update correctly."


def test_set_unknown_param():
    with pytest.raises(ValueError) as excinfo:
        piqtree2.set_params({"unknown_param": True})
    assert "unknown parameter" in str(
        excinfo.value
    ), "set_params: No error raised for unknown parameter."


def test_get_unknown_param():
    result = piqtree2.get_param("unknown_param")
    assert (
        result is None
    ), "get_param: Expected None for unknown parameter, got {}".format(result)


def test_set_invalid_param_type():
    with pytest.raises(TypeError) as excinfo:
        piqtree2.set_params({"fai": "not a boolean"})
    assert "Incorrect type" in str(
        excinfo.value
    ), "set_params: No error raised for incorrect parameter type."


# Reset the global state after each test
@pytest.fixture(autouse=True)
def run_around_tests():
    # Before each test, set the params to a known state
    piqtree2.set_params({"fai": False})

    yield

    # After each test, you can reset to the default params if needed
    # piqtree2.set_params({"fai": False})
