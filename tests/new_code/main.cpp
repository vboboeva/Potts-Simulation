#include <iostream>
#include <random>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "vlc_pnet.h"

int main(int argc, char *argv[]){

    std::cout << "Potts simulation" << std::endl;

    /***************************************************************************
    PATTERN GENERATION
    ***************************************************************************/
    //Random seed init
    std::default_random_engine generator;
    generator.seed(12345);

    PatternGen pgen(
               600, //N
               10, //p
               3, //S
               0.25, //a
               11.0, //beta
               50, //N_fact
               200, //Num_fact
               0.25, //a_fact
               1e-6, //eps
               3e-6, //a_pf
               3e-6 //fact_eigen_slope
               );

    pgen.set_random_generator(generator);
    pgen.generate();
    pgen.save_pattern_to_file("patterns.dat");

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/

    //Reset generator, this is because the old code run 2 different files and
    //generators. This is just to check that everything is as in the old code.
    generator.seed(12345);

    //Create the network
    VLC_PNet pnet(600,
                 90,
                 3
                 );


    //Connect units (generate connection matrix)
    pnet.connect_units(generator);
    //Initialize the network
    pnet.init_network(11.0,0.1,10,0.25,pgen.get_patt());


    //Write states to file
    pnet.save_states_to_file("init_states.dat");

    //Write connections to file
    pnet.save_connections_to_file("init_connections.dat");

    //Write the J tensor file
    pnet.save_J_to_file("init_J.dat");

    //Start the dynamics

    pnet.start_dynamics(generator,
                        10,
                        10000, //tstatus (tempostampa)
                        100,  //Number of updates
                        pgen.get_patt(),
                        1,
                        0.25,
                        0.1,
                        0.8,
                        5.0,
                        10*600, //tau
                        0.3, //b1
                        0.01, //b2
                        0.000001, //b3
                        11.0, //beta
                        500*600 //tx (n0)
                        );

    //Check states
    pnet.save_states_to_file("updated_states.dat");

    std::cout << "End of the simulation" << std::endl;
    return 0;
}
