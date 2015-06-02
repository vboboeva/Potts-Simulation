#include "main.h"


int main(int argc, char *argv[]){

    std::cout << "Welcome to Potts simulation" << std::endl;

    //Random seed init
    std::default_random_engine generator;
    generator.seed(12345);

    PatternGen pgen(
               600,
               10,
               3,
               0.25,
               11.0,
               50,
               200,
               0.25,
               1e-6,
               3e-6,
               3e-6
               );

    pgen.set_random_generator(generator);
    pgen.generate();
    pgen.eval_stats();
    pgen.save_pattern_to_file("patterns.dat");

    std::cout << "End" << std::endl;
    return 0;
}
