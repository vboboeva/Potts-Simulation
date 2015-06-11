#include "main.h"


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
    pgen.eval_stats();
    pgen.save_pattern_to_file("patterns.dat");

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/

    //Generate random sequence
    RandomSequence sequence(600);
    sequence.shuffle(generator);


    //Create Potts Network handler
    PNetwork network(600,3,90);



    std::cout << "End of the simulation" << std::endl;
    return 0;
}
