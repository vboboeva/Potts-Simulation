#include <iostream>
#include <random>

#include "pattern_generation.h"
#include "random_sequence.h"
#include "p_unit.h"
#include "p_network.h"
#include "parameters_struct.h"
#include "utils.h"

int main(int argc, char *argv[]){

    std::cout << "Potts simulation" << std::endl;

    /***************************************************************************
    PATTERN GENERATION
    ***************************************************************************/
    //Random seed init
    std::default_random_engine generator;
    generator.seed(12345);

    std::cout << "loading parameters" << std::endl;
    struct parameters params;
    load_params("params.cfg", params);

    PatternGen pgen(
               params.N, //N
               params.p, //p
               params.S, //S
               params.a, //a
               params.beta, //beta
               params.N_fact, //N_fact
               params.Num_fact, //Num_fact
               params.a_fact, //a_fact
               params.eps, //eps
               params.a_pf, //a_pf
               params.fact_eigen_slope //fact_eigen_slope
               );

    pgen.set_random_generator(generator);
    pgen.generate();

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/

    //Create the network
    PNetwork pnet(pgen, //Patterns
                    params.C, //Number of connections for each unit
                    params.U, //U
                    params.w, //w
                    params.g //g
                    );

    //Initialize the network
    pnet.init_units();


    //Start the dynamics
    pnet.start_dynamics(params.nupdates, //Number of updates
                        500*params.N, //n0 500*N
                        10*params.N, //tau 10*N
                        params.b1, //b1
                        params.b2, //b2
                        params.b3, //b3
                        1 //pattern number
                        );


    std::cout << "End of the simulation" << std::endl;
    return 0;
}
