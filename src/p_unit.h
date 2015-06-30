
#ifndef __P_UNIT
#define __P_UNIT

class PUnit{
    private:
        double * state; //Array that keeps the value of the states of the current Potts unit
        double * cdata; //Array that keeps for each array all the data necessary to update the unit.
        double * r; //Input!?
        double * h;
        double * theta;

        int S; //Number of states
        int C; //Number of connections
    public:
        ~PUnit();
        PUnit(const int S, const int C);
        void init_states(const double beta, const double U);
        void init_J(const int p, const double a, const int * xi, const int unit, const int * cm, PUnit ** network);
        double * get_state();
        void update_rule(const int init_pattern,
                        const double U,
                        const double w,
                        const double g,
                        const double tau,
                        const double b1,
                        const double b2,
                        const double b3,
                        const double beta,
                        const int tx,
                        const int t
                        );
        void update_cdata(const double * new_states, const int index);

        double * get_cdata(){ return this->cdata;};
};

#endif
