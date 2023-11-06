// In params.h
#ifndef PARAMS_H
#define PARAMS_H

#include <string>

void set_params(const std::string& json_str);
std::string get_params();
std::string get_param(const std::string& name);

#endif // PARAMS_H
