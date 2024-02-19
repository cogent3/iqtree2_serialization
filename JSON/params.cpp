#include "params.h"
#include "utils/tools.h"
#include "nlohmann/json.hpp"  
using json = nlohmann::json;

void set_params(const std::string& json_str) {
    try {
        Params& params = Params::getInstance();
        json j = json::parse(json_str);  // Parse JSON string
        params.from_json(j);  // Update Params with JSON data
    } catch (const json::exception& e) {
        // Convert the exception into a Python exception
        throw std::runtime_error("Error setting parameters: " + std::string(e.what()));
    }
}

std::string get_params() {
    Params& params = Params::getInstance();
    json j = params.to_json();
    return j.dump();  // Convert JSON to string and return
}

std::string get_param(const std::string& name) {
    try {
        Params& params = Params::getInstance();
        json j = params.get_param(name.c_str());
        return j.dump();  // Convert JSON to string and return
    } catch (const std::exception& e) {
        // Convert the exception into a Python exception
        throw std::runtime_error("Error getting parameter: " + std::string(e.what()));
    }
}
