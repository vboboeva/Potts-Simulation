
#ifndef __PNET_H
#define __PNET_H

#include <fstream>
#include <random>
#include "config.h"

/**********************************
Abstract class to constrain important members and methods in such a way all the
PNet family can be handled with those methods
**********************************/

class PNet{
    protected:
        int N;
        virtual void evaluate_m(const int & p, const __fpv & a, const int * xi, __fpv m[]) = 0;
        virtual void init_J(const int & p, const __fpv & a, const int * xi) = 0;
        void get_status(const int & p, const int & tx, const int & t, const int * xi, const __fpv & a, int & Mumaxold, int & Mumax, int & steps, bool & stop);

    public:

        virtual void print_cm() = 0;
        virtual void save_states_to_file(const std::string & filename) = 0;
        virtual void save_connections_to_file(const std::string & filename) = 0;
        virtual void save_J_to_file(const std::string & filename) = 0;


        virtual void init_network(const double beta,
                          const double U,
                          const int & p,
                          const __fpv & a,
                          const int * xi
                          ) = 0;

        virtual void start_dynamics(const std::default_random_engine & generator,
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
                        ) = 0;
};

#endif
