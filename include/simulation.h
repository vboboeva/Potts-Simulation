#ifndef __SIMULATION_H
#define __SIMULATION_H

#include <string>
#include "parameters_struct.h"

void PottsSim(struct parameters params, std::string filename, const int & id = 0, std::string mode = "auto");

#endif
