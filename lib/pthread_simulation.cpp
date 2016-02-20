#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <pthread.h>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "lc_pnet_reduced.h"
#include "parameters_struct.h"
#include "simulation.h"
#include "config.h"

struct thread_data{
   int thread_id;
   int total_threads;
};

struct global_data{
    struct parameters params;
    __fpv * J; //Global heap J
    int * cm; //Global heap cm
    int * xi; //Global heap xi
};

struct global_data gp;
struct thread_data * thread_data_array;

void *fthreads(void *threadarg)
{
    struct thread_data *d;
    d = (struct thread_data *) threadarg;

    struct parameters params = gp.params;
    int * a;
    __fpv * b;
    int rem, patt, num_sim, i;
    rem = params.p % d->total_threads;
    num_sim = params.p / d->total_threads;

    if(d->thread_id < rem){
        num_sim++;
    }

    std::default_random_engine generator;
    //std::cout << "ID: " << d->thread_id << " NUM_SIM: " << num_sim << std::endl;



    LC_PNet_Reduced mysim(params.N,params.C,params.S,a,b);

    mysim.set_J(gp.J);
    mysim.set_cm(gp.cm);
    
    for( i = 0; i < num_sim; ++i){
        patt = d->thread_id + d->total_threads;

        mysim.init_states(params.beta,params.U);
        //std::cout << "XI0: " << gp.xi[1] << " J: " << gp.J[1] << std::endl;
        mysim.start_dynamics(generator,
                            params.p,
                            params.tstatus, //tstatus (tempostampa)
                            params.nupdates,  //Number of updates
                            gp.xi,
                            patt, //Pattern cued
                            params.a,
                            params.U,
                            params.w,
                            params.g,
                            params.tau * params.N, //tau
                            params.b1, //b1
                            params.b2, //b2
                            params.b3, //b3
                            params.beta, //beta
                            500*params.N //tx (n0)
                            );
    }

    pthread_exit(NULL);
}

void PottsSim(struct parameters params, const int & threads){

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

    int i;
    thread_data_array = new struct thread_data[threads];

    for(i = 0; i < threads; ++i){
        thread_data_array[i].thread_id = i;
        thread_data_array[i].total_threads = threads;
    }

    //Random seed init
    std::default_random_engine generator;

    /***************************************************************************
    INITIALIZATION
    ***************************************************************************/
    PatternGen pgen(
               params.N, //N
               params.p, //p
               params.S, //S
               params.a, //a
               params.beta, //beta
               params.N_fact, //N_fact
               params.Num_fact, //Num_fact
               params.a_fact, //a_fact
               params.eps, //eps
               params.a_pf, //a_pf
               params.fact_eigen_slope //fact_eigen_slope
               );

    pgen.generate();

    //Seed for the update shuffle
    //generator.seed(time(NULL));

    //Create the network
    LC_PNet pnet(params.N,
                params.C,
                params.S
                    );

    //Connect units
    pnet.connect_units(generator);
    pnet.init_network(params.beta,params.U,params.p,params.a,pgen.get_patt());

    /***************************************************************************
    DYNAMICS
    ***************************************************************************/
    std::cout << "STARTING DYNAMICS" << std::endl;
    //Start the dynamics

    gp.params = params;
    gp.J = pnet.get_J();
    gp.xi = pgen.get_patt();
    gp.cm = pnet.get_cm();

    t1 = std::chrono::high_resolution_clock::now();

    int rc;
    void *status;
    pthread_t * sthreads = new pthread_t[threads];
    pthread_attr_t attr;

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_init(&attr);

    for(i = 0; i < threads; ++i){
        rc = pthread_create(&sthreads[i], &attr, fthreads, (void *)&thread_data_array[i]);
        if(rc){
            std::cout << "ERROR; return code from pthread_create() is: " << rc << std::endl;
        }
    }

    pthread_attr_destroy(&attr);

    for(i = 0; i < threads; ++i) {
        rc = pthread_join(sthreads[i], &status);
        if(rc) {
            std::cout << "ERROR; return code from pthread_join() is " << rc << std::endl;
        }
    }

    t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    std::cout << "DURATION: " << duration << std::endl;

}
