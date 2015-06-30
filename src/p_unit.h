
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

        int S; //Number of states
        int C; //Number of connections

    public:
        ~PUnit();
        PUnit(const int & S, const int & C);
        void init_states(const __fpv & beta, const __fpv & U);
        void init_J(const int &p, const __fpv &a, const int * xi, const int &unit, const int * cm, PUnit ** network);
        __fpv * get_state();
        void update_rule(const int & init_pattern,
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
        void update_cdata(const __fpv * new_states, const int & index);
        __fpv * get_cdata(){ return this->cdata;};
};

#endif
