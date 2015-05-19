#include "main.h"


int main(int argc, char *argv[]){

    std::cout << "Welcome potts simulation" << std::endl;

    //Random seed init
    std::default_random_engine generator;
    generator.seed(time(NULL));



    std::cout << "End" << std::endl;
    return 0;
}
