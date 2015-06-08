#include <iostream>
#include <random>
#include <time.h>
#include <iomanip>
#include <fstream>
#include <algorithm>

//pattern_generation.cpp
#ifndef __PATTERN_GENERATION
#define __PATTERN_GENERATION

class PatternGen{
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

};

#endif

//dynamics.cpp
#ifndef __DYNAMICS
#define __DYNAMICS

class RandomSequence{
    private:
        int * sequence;
        int N;

    public:
        RandomSequence(const int N);
        ~RandomSequence();

        void shuffle(std::default_random_engine & generator);
        void save_sequence_to_file();
        void print();
        int * begin();
        int * end();

};

#endif
