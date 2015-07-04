#include <iostream>
#include <random>

#include "pattern_generation.h"
#include "random_sequence.h"
#include "p_unit.h"
#include "p_network.h"

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

    //Generate random sequence
    RandomSequence sequence(600);
    sequence.shuffle(generator);
    //sequence.print(); //If you want to check the output of the shuffled sequence.

    //Reset generator, this is because the old code run 2 different files and
    //generators. This is just to check that everything is as in the old code.
    generator.seed(12345);

    //Create the network
    PNetwork pnet(pgen, //Patterns
                    90, //Number of connections for each unit
                    0.1, //U
                    0.8, //w
                    5.0 //g
                    );

    //Initialize the network
    pnet.init_units();//Yet not fully implemented

    //Write states to file
    pnet.save_states_to_file("init_states.dat");

    //Write connections to file
    pnet.save_connections_to_file("init_connections.dat");

    //Write the J tensor file
    pnet.save_J_to_file("init_J.dat");

    //Start the dynamics

    pnet.start_dynamics(100, //Number of updates
                        500*600, //n0
                        10*600, //tau
                        0.3, //b1
                        0.01, //b2
                        0.000001, //b3
                        1 //pattern number
                        );

    //Check states
    pnet.save_states_to_file("updated_states.dat");

    std::cout << "End of the simulation" << std::endl;
    return 0;
}
