#ifndef __HC_PNET_H
#define __HC_PNET_H

#include <random>

#include "pnet.h"

/*! Class defining the High connectivity network */
class HC_PNet : public PNet {
    private:
        //int N; //Number of units (Now defined on parent class PNet)
        int C; /*!< Number of connections per unit */
        int S; /*!< Number of states per unit */
        int * cm; /*!< Connectivity matrix */
        __fpv * J; /*!< J tensor */
        __fpv * active_states; // N * S
        __fpv * inactive_states; // N
        int * ucm; //1-0 connection matrix
        __fpv * active_r; //inputs array
        __fpv * inactive_r;
        __fpv * h;
        __fpv * theta;
        int * xi;


        void init_states(const __fpv & beta, const __fpv & U);
        void update_rule(const int & unit,
                         const int & pattern,
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

        void evaluate_m(const int & p, const __fpv & a, const int * xi, __fpv m[]);
        void init_J(const int & p, const __fpv & a, const int * xi);



    public:
        HC_PNet(const int & N, const int & C, const int & S);
        ~HC_PNet();

        void import_connections(const std::string & filename);
        void print_cm();
        void save_states_to_file(const std::string & filename);
        void save_connections_to_file(const std::string & filename);
        void save_J_to_file(const std::string & filename);


        void connect_units(std::default_random_engine & generator);

        void init_network(const __fpv & beta,
                          const __fpv & U,
                          const int & p,
                          const __fpv & a,
                          const int * xi
                          );
        void start_dynamics(std::default_random_engine & generator,
                            const int & p,
                            const int & tstatus,
                            const int & nupdates,
                            const int * xi,
                            const int & pattern,
                            const __fpv & a,
                            const __fpv & U,
                            const __fpv & w,
                            const __fpv & g,
                            const __fpv & tau,
                            const __fpv & b1,
                            const __fpv & b2,
                            const __fpv & b3,
                            const __fpv & beta,
                            const int & tx
                            );

};

#endif
