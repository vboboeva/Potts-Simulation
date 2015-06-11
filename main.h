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

class PUnit{
    private:
        double * state; //Array that keeps the value of the states of the current Potts unit
        double * cdata; //Array that keeps for each array all the data necessary to update the unit.
        double * r; //Input!?
        int S; //Number of states
        int C; //Number of connections
    public:
        ~PUnit();
        PUnit(const int S, const int C);
        void init(const double beta, const double U);
};

class PNetwork{
    private:
        PUnit ** network;
        int N; //Number of units
        int S; //Number of states per unit
        int C; // Number of connections per unit
        double beta;
        double U;

        int * cm; //Connection matrix

    public:
        PNetwork(const int N, const int S, const int C);
        ~PNetwork();
        void Init_Units();
        void ConnectUnits();
        void start();




};

/*
struct data{
        double * Jjl; //Array keeping the part of the connection tensor Jikjl with fixed i and k
        double * Sjl; //Array keeping the state l of the connected unit j;
};
*/


#endif
