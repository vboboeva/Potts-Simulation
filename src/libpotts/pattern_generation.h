#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <iomanip>

#include "p_network.h"

#ifndef __PATTERN_GENERATION
#define __PATTERN_GENERATION

class PatternGen{

    friend class PNetwork; //Allows PNetwork to direct access PatternGen member

    private:
        int N; // Number of potts units
        int p; // Number of patterns
        int S; // Number of states
        double a;
        double beta;
        int N_fact;
        int Num_fact;
        double a_fact;
        double eps;
        double a_pf;
        double fact_eigen_slope;

        int * Patt; // Dynamic array of dimension p * N

        std::default_random_engine * generator;

    public:
        PatternGen(const int N,
                   const int p,
                   const int S,
                   const double a,
                   const double beta,
                   const int N_fact,
                   const int Num_fact,
                   const double a_fact,
                   const double eps,
                   const double a_pf,
                   const double fact_eigen_slope
                   );
        ~PatternGen();

        void set_random_generator(std::default_random_engine & generator);
        void generate();
        void eval_stats();
        void save_pattern_to_file(const std::string filename);
        int * get_patt();
        int * get_patt(const int n);

};

#endif
