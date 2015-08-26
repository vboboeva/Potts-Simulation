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

    pgen.generate();
    pgen.save_pattern_to_file("patterns.dat");

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/

    //Reset generator, this is because the old code run 2 different files and
    //generators. This is just to check that everything is as in the old code.
    generator.seed(12345);

    //Create the network
    LC_PNet pnet(params.N,
                params.C,
                params.S
                    );


    //Connect units (generate connection matrix)
    //pnet.connect_units(generator);
    pnet.import_connections("../old_code/init_connections2.dat");
    //Initialize the network
    pnet.init_network(params.beta,params.U,params.p,params.a,pgen.get_patt());

    //Write states to file
    pnet.save_states_to_file("init_states.dat");

    //Write connections to file
    pnet.save_connections_to_file("init_connections.dat");

    //Write the J tensor file
    pnet.save_J_to_file("init_J.dat");

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

    //Check states
    pnet.save_states_to_file("updated_states.dat");

    //std::cout << "End of the simulation" << std::endl;
    return 0;
}
