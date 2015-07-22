
#ifndef __P_UNIT
#define __P_UNIT
#include <vector>
#include "config.h"

class PUnit{

    friend class PNetwork;

    private:
        __fpv * state; //Array that keeps the value of the states of the current Potts unit
        __fpv * cdata; //Array that keeps for each array all the data necessary to update the unit.
        __fpv * r; //Input!?
        __fpv * h;
        __fpv * theta;
        __fpv * lc_cdata;

        int S; //Number of states
        int C; //Number of connections
        int N; //Number of Units of the network

    public:
        ~PUnit();
        PUnit(const int & S, const int & C, const int & N);
        void init_states(const __fpv & beta, const __fpv & U);
        void init_J(const int &p, const __fpv &a, const int * xi, const int &unit, const int * ucm, PUnit ** network);

        __fpv * get_state();
        void update_rule(const int & init_pattern, const __fpv * states,
                        const __fpv & U,
                        const __fpv & w,
                        const __fpv & g,
                        const __fpv & tau,
                        const __fpv & b1,
                        const __fpv & b2,
                        const __fpv & b3,
                        const __fpv & beta,
                        const int & tx,
                        const int & t
                        );

        __fpv * get_cdata(){ return this->cdata;};
        __fpv * get_lc_cdata(){ return this->lc_cdata;};

        void lc_init_J(const int &p, const __fpv &a, const int * xi, const int &unit, const int * cm, PUnit ** network);
        void lc_update_rule(const int & init_pattern, const __fpv buffer[],
                        const __fpv & U,
                        const __fpv & w,
                        const __fpv & g,
                        const __fpv & tau,
                        const __fpv & b1,
                        const __fpv & b2,
                        const __fpv & b3,
                        const __fpv & beta,
                        const int & tx,
                        const int & t
                        );
};

#endif
