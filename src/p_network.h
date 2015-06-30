
#ifndef __P_NETWORK_H
#define __P_NETWORK_H

#include <string>
#include <vector>

#include "pattern_generation.h"
#include "p_unit.h"



struct uindx{
    int unit;
    int idx;
};

class PNetwork{
    private:
        PUnit ** network;
        int N; //Number of units
        int S; //Number of states per unit
        int C; // Number of connections per unit
        int p; //Number of patterns
        double beta;
        double U;
        double w;
        double g;

        int * cm; //Connection matrix
        std::vector<uindx> * icm;
        double * m; //m
        PatternGen * pgen;

    public:
        PNetwork(PatternGen & pgen,const int C, const double U, const double w, const double g);
        ~PNetwork();
        void init_units();
        void connect_units();
        void evaluate_m();
        void start_dynamics(const int nupdates, const int tx, const double tau, const double b1, const double b2, const double b3, const int pattern_number);

        void print_cm();
        void save_states_to_file(std::string filename);
        void save_connections_to_file(std::string filename);
        void save_J_to_file(std::string filename);

};

#endif
