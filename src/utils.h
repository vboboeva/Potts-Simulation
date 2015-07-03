#include <string>
#include "parameters_struct.h"

void key_value_matcher(std::string key, std::string value, struct parameters & params);
void load_params(std::string filename, struct parameters & params);
void print_params(struct parameters & params);
