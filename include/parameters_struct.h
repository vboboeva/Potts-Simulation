
#ifndef __PARAMETERS_STRUCT
#define __PARAMETERS_STRUCT

#include "config.h"

struct parameters{
    int N;
    int C;
    int p;
    int S;
    int nupdates;
    int NumSet;
    int N_fact;
    int Num_fact;
    int tstatus;
    __fpv a;
    __fpv U;
    __fpv b1;
    __fpv b2;
    __fpv b3;
    __fpv beta;
    __fpv w;
    __fpv g;
    __fpv tau;
    __fpv a_fact;
    __fpv eps;
    __fpv a_pf;
    __fpv fact_eigen_slope;
};

#endif
