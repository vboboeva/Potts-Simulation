#include <iostream>
#include <random>
#include <time.h>
#include <iomanip>


//pattern_generation.cpp
#ifndef __PATTERN_GENERATION
#define __PATTERN_GENERATION

class PatternGen{
    private:
        int p; // Number of patterns
        int N; // Number of potts units
        int S; // Number of states
        int N_fact;
        int Num_fact;
        double eps;
        double a_pf;
        double piccolo;
        double fact_eigen_slope;
        double a_fact;
        double beta;
        double a;

        int * Factors; // Dynamic array of dimension N_fact * Num_fact
        int * Patt; // Dynamic array of dimension p * N

    public:
        PatternGen(int N_fact, int Num_fact, int p, int N, int S, double eps, double a_pf, double piccolo, double fact_eigen_slope);
        void generate();
        void set_random_generator(std::default_random_engine & generator);
        void eval_stats();

};

#endif
