#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <time.h>
#include "master.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

int old_code( int pid)
{
int k;
FILE *last;
FILE *ksequenza;
FILE *Passi;


system(("mkdir sim_"+std::to_string(pid)).c_str());

system(("icc -O3 genero_pattern.cpp -o sim_"+std::to_string(pid)+"/gen.out -std=c++11").c_str());
system(("icc -O3 main_restyle.cpp -D_NO_END_CONDITION -o sim_"+std::to_string(pid)+"/main.out -std=c++11").c_str());


printf("genero %d\n",k);


system(("sim_"+std::to_string(pid)+"/gen.out").c_str());


system(("sim_"+std::to_string(pid)+"/main.out").c_str());

return 0;

}
