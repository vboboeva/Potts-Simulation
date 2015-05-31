#include <iostream>
#include <random>
#include <time.h>
#include <iomanip>


//pattern_generation.cpp
#ifndef __PATTERN_GENERATION
#define __PATTERN_GENERATION

class PatternGen{
    private:
        int p;
        int N;
        int N_fact;
        int Num_fact;
        std::default_random_engine * gen; //Generator of random values
        int * Factors;
        int * Patt;

    public:
        PatternGen(int N_fact, int Num_fact, int p, int N);
        void generate();
        void set_random_generator(std::default_random_engine & generator);
        void eval_stats();

};

#endif
