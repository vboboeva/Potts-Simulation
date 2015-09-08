#include <iostream>
#include <random>
#include <chrono>

#include "pattern_gen.h"
#include "random_seq.h"
#include "lc_pnet.h"
#include "parameters_struct.h"
#include "utils.h"

int main(int argc, char *argv[]){

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::high_resolution_clock::time_point t3;
    std::chrono::high_resolution_clock::time_point t4;

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

    pgen.generate();
    //Create the network
    generator.seed(12345);


    LC_PNet pnet(params.N,
                params.C,
                params.S
                    );

    //Connect units
    pnet.connect_units(generator);
    t2 = std::chrono::high_resolution_clock::now();
    ////////////////////////////////////////////////////////////////////////////
    pnet.import_connections("../old_code/init_connections2.dat");
    ////////////////////////////////////////////////////////////////////////////
    t3 = std::chrono::high_resolution_clock::now();
    pnet.init_network(params.beta,params.U,params.p,params.a,pgen.get_patt());
    t4 = std::chrono::high_resolution_clock::now(); //STOP TIMER
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( (t4 - t3) + (t2 - t1) ).count();
    std::cout << "INITIALIZATION ELAPSED TIME(ms): "<< duration << std::endl;

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/
    t1 = std::chrono::high_resolution_clock::now(); //START TIMER


    //Start the dynamics
    pnet.start_dynamics(generator,
                        params.p,
                        params.tstatus, //tstatus (tempostampa)
                        params.nupdates,  //Number of updates
                        pgen.get_patt(),
                        0, //Pattern number
                        params.a,
                        params.U,
                        params.w,
                        params.g,
                        params.tau * params.N, //tau
                        params.b1, //b1
                        params.b2, //b2
                        params.b3, //b3
                        params.beta, //beta
                        500*params.N //tx (n0)
                        );

    t2 = std::chrono::high_resolution_clock::now(); //STOP TIMER
    duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "TOTAL UPDATE ELAPSED TIME(ms): "<< duration << std::endl;


    std::cout << "End of the simulation" << std::endl;
    return 0;
}
