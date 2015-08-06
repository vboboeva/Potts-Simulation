#include <iostream>
#include <random>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "vlc_pnet.h"
#include "utils.h"
int main(int argc, char *argv[]){

    std::cout << "Potts simulation" << std::endl;

    /***************************************************************************
    PATTERN GENERATION
    ***************************************************************************/
    //Random seed init
    std::default_random_engine generator;
    generator.seed(12345);

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
    HC_PNet pnet(params.N,
                params.C,
                params.S
                    );


    //Connect units (generate connection matrix)
    pnet.connect_units(generator);

    //Initialize the network
    pnet.init_network(params.beta,params.U,params.p,params.a,pgen.get_patt());

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

    std::cout << "End of the simulation" << std::endl;
    return 0;
}
