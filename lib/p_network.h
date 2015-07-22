
#ifndef __P_NETWORK_H
#define __P_NETWORK_H

#include <string>
#include <vector>

#include "config.h"
#include "pattern_generation.h"
#include "p_unit.h"


class PNetwork{
    private:
        PUnit ** network;
        int N; //Number of units
        int S; //Number of states per unit
        int C; // Number of connections per unit
        int p; //Number of patterns
        __fpv beta;
        __fpv U;
        __fpv w;
        __fpv g;

        int * cm; //Connection matrix
        int * ucm; //1-0 connection matrix
        __fpv * m; //m
        __fpv * states;
        PatternGen * pgen;

    public:
        PNetwork(PatternGen & pgen,const int & C, const __fpv & U, const __fpv & w, const __fpv & g);
        ~PNetwork();
        void init_units();
        void connect_units();
        void evaluate_m();
        void start_dynamics(const int & nupdates, const int & tx, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const int & pattern_number);

        void print_cm();
        void save_states_to_file(const std::string & filename);
        void save_connections_to_file(const std::string & filename);
        void save_J_to_file(const std::string & filename);

        void lc_init_units();
        void lc_start_dynamics(const int & nupdates, const int & tx, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const int & pattern_number);

};

#endif
