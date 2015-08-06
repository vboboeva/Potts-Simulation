#include <iostream>
#include <random>
#include <chrono>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "vlc_pnet.h"
#include "parameters_struct.h"
#include "utils.h"

int main(int argc, char *argv[]){

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

    std::cout << "Potts simulation" << std::endl;

    /***************************************************************************
    LOADING PARAMETERS
    ***************************************************************************/

    //Random seed init
    std::default_random_engine generator;
    generator.seed(12345);

    std::cout << "loading parameters" << std::endl;
    struct parameters params;
    load_params("params.cfg", params);

    /***************************************************************************
    INITIALIZATION
    ***************************************************************************/
    t1 = std::chrono::high_resolution_clock::now(); //START TIMER
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


    //Create the network
    HC_PNet pnet(pgen, //Patterns
                    params.C, //Number of connections for each unit
                    params.U, //U
                    params.w, //w
                    params.g //g
                    );

    //Initialize the network
    pnet.init_units();

    t2 = std::chrono::high_resolution_clock::now(); //STOP TIMER
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "INITIALIZATION ELAPSED TIME(ms): "<< duration << std::endl;

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/
    t1 = std::chrono::high_resolution_clock::now(); //START TIMER

    //Start the dynamics
    pnet.start_dynamics(params.nupdates, //Number of updates
                        500*params.N, //n0 500*N
                        10*params.N, //tau 10*N
                        params.b1, //b1
                        params.b2, //b2
                        params.b3, //b3
                        1 //pattern number
                        );

    t2 = std::chrono::high_resolution_clock::now(); //STOP TIMER
    duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "TOTAL UPDATE ELAPSED TIME(ms): "<< duration << std::endl;


    std::cout << "End of the simulation" << std::endl;
    return 0;
}