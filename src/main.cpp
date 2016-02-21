#include <iostream>

#include "utils.h"
#include "pthread_simulation.h"
#include "simulation.h"
#include "parameters_struct.h"


int main(int argc, char *argv[]){

    std::cout << "Potts simulation" << std::endl;

    struct parameters params;
    load_params("params.cfg", params);

    PottsSim(params,0,50);

    //ThreadedPottsSim(params,7,50);

    std::cout << "End of the simulation" << std::endl;
    return 0;
}
