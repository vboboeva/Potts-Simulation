#ifndef __PATTERN_GEN_H
#define __PATTERN_GEN_H

#include <string>
#include <random>

#include "config.h"

class PatternGen{

    friend class PNetwork; //Allows PNetwork to direct access PatternGen member

    private:
        int N; // Number of potts units
        int p; // Number of patterns
        int S; // Number of states
        __fpv a;
        __fpv beta;
        int N_fact;
        int Num_fact;
        __fpv a_fact;
        __fpv eps;
        __fpv a_pf;
        __fpv fact_eigen_slope;

        int * Patt; // Dynamic array of dimension p * N

    public:
        PatternGen(const int N,
                   const int p,
                   const int S,
                   const __fpv a,
                   const __fpv beta,
                   const int N_fact,
                   const int Num_fact,
                   const __fpv a_fact,
                   const __fpv eps,
                   const __fpv a_pf,
                   const __fpv fact_eigen_slope
                   );
        ~PatternGen();

        void generate();
        void eval_stats();
        void save_pattern_to_file(const std::string filename);
        void import_pattern(const std::string filename);
        int * get_patt();
        int * get_patt(const int n);

};

#include "pnet.h"

#endif
