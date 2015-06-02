#include "main.h"


int main(int argc, char *argv[]){

    std::cout << "Welcome to Potts simulation" << std::endl;

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

    std::cout << "End" << std::endl;
    return 0;
}
