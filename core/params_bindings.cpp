#include <pybind11/pybind11.h>
#include "params.h"
#include "nlohmann/json.hpp"
#include "utils/tools.h"

using json = nlohmann::json;
namespace py = pybind11;

PYBIND11_MODULE(params_bindings, m) {
    m.doc() = "Python bindings for the IQ-TREE parameter handling";

    m.def("set_params", [](const std::string& json_str) {
        Params& params = Params::getInstance();
        json j = json::parse(json_str);
        params.from_json(j);
    }, "Set the parameters for IQ-TREE from a JSON string.");

    m.def("get_params", []() -> std::string {
        Params& params = Params::getInstance();
        json j = params.to_json();
        return j.dump();
    }, "Get the current parameters for IQ-TREE as a JSON string.");

    m.def("get_param", [](const std::string& name) -> std::string {
        Params& params = Params::getInstance();
        json j = params.get_param(name);
        return j.dump();
    }, "Get the value of a specific parameter for IQ-TREE as a JSON string.");
}
