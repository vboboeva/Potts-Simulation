#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <pthread.h>
#include <cstring>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "parameters_struct.h"
#include "simulation.h"
#include "config.h"

//Struct definition for the data to be passed as arguments to the threads
struct thread_data{
   int thread_id; //Will have the thread id
   int total_threads; //Will store the total number of threads
};

//Struct definition of the simulation values to be shared
struct global_data{
    //Stores an hard copy of the parameters
    struct parameters params;
    //Links to the initial arrays
    ////////////////////////////
    __fpv * J;
    __fpv * i_active_r;
    __fpv * i_theta;
    __fpv * i_active_states;
    __fpv * i_inactive_states;
    __fpv * i_inactive_r;
    int * cm;
    int * ucm;
    int * xi;
    //*************************
    bool stop; //Flag where all the threads will look in order to stop
};

//Files
std::ofstream ksequence;
std::ofstream msequence;
std::ofstream llength;

int max_length_counter; //Counter which stores: "How many different cues take to the maximum length?"
int max_sim_max_length; //Value that stores the maximum number of maximum lenght
//simulation before the sim exits sayn "ok, for any other cue the sim is infinite"

struct global_data gp; //Instance of the global data of the simulation
struct thread_data * thread_data_array; //Pointer to link to the array of arguments to be passed to the threads


pthread_mutex_t kseq,mseq,llen,inf; //Different mutexes for file or variable to lock


//Thread function
void *fthreads(void *threadarg){

    struct thread_data *d;
    d = (struct thread_data *) threadarg;

    struct parameters params = gp.params;
    int p_cued = params.p;
    //If the number of pattern is higher than 100 simply run the sim for 100 different cues
    if(params.p >= 100) p_cued = 100;
    p_cued = 10;

    int rem, patt, num_sim, i;
    rem = p_cued % d->total_threads;
    num_sim = p_cued / d->total_threads;

    if(d->thread_id < rem){
        num_sim++;
    }

    std::default_random_engine generator;
    std::cout << "ID: " << d->thread_id << " NUM_SIM: " << num_sim << std::endl;

    std::vector<__fpv>::iterator l;
    std::vector<int>::iterator k;
    std::vector<__fpv>::iterator m;

    LC_PNet mysim(params.N,params.C,params.S,gp.cm,gp.ucm,gp.J);
    mysim.stop = &gp.stop; //Set the parameter to watch to stop the simulation

    //Evaluate the size in bytes of the active states, theta and r arrays
    int cp = sizeof(__fpv)*params.N * params.S, cpinactive = sizeof(__fpv)*params.N;


    for( i = 0; i < num_sim; ++i){

        patt = d->thread_id + i*d->total_threads;

        //In order to have the same result as with the Psim without threads remember to reset the seed to the
        //same value each time
        generator.seed(12345);
        //Automatic reset (slower, it's better copy the initial values from the global arrays)
        //mysim.reset(params.beta,params.U);

        //Manual reset (copy from the global arrays)
        mysim.ksequence.clear();
        mysim.msequence.clear();
        mysim.infinite = false;
        std::memcpy(mysim.get_active_states(),gp.i_active_states,cp);
        std::memcpy(mysim.get_active_r(),gp.i_active_r,cp);
        std::memcpy(mysim.get_theta(),gp.i_theta,cp);
        std::memcpy(mysim.get_inactive_r(),gp.i_inactive_r,cpinactive);
        std::memcpy(mysim.get_inactive_states(),gp.i_inactive_states,cpinactive);

        //Start the dynamics passing the global array of patterns
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

        //Lock everything before doing the check, so that we are sure we are the first.
        pthread_mutex_lock(&inf);
        if(gp.stop){
            pthread_mutex_unlock(&inf);
            break;
        }else if(mysim.infinite){
            max_length_counter++;
            if(max_length_counter == max_sim_max_length){
                gp.stop = true;
            }
        }
        pthread_mutex_unlock(&inf);

        // std::cout << "thread id: " << d->thread_id << " ";
        // mysim.print_ksequence();

        //Write the ksequence
        //////////////////////////
        pthread_mutex_lock(&kseq);
        ksequence << patt << " ";
        k = mysim.ksequence.begin();
        while( k!= mysim.ksequence.end()){
            ksequence << *k << " ";
            k++;
        }
        ksequence << std::endl;
        pthread_mutex_unlock(&kseq);
        //**************************

        //Write the msequence
        //////////////////////////
        pthread_mutex_lock(&mseq);
        msequence << patt << " ";
        m = mysim.msequence.begin();
        while( m!= mysim.msequence.end()){
            msequence << *m << " ";
            m++;
        }
        msequence << std::endl;
        pthread_mutex_unlock(&mseq);
        //**************************

        //Write the latching length
        //////////////////////////
        pthread_mutex_lock(&llen);
        llength << patt << " ";
        llength << mysim.latching_length << std::endl;
        pthread_mutex_unlock(&llen);
        //**************************
    }
    pthread_exit(NULL);
}

void ThreadedPottsSim(struct parameters params, const int & threads, const int & msml){

    int i;

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

    t1 = std::chrono::high_resolution_clock::now();

    max_length_counter = 0;
    max_sim_max_length = msml;

    thread_data_array = new struct thread_data[threads];
    for(i = 0; i < threads; ++i){
        thread_data_array[i].thread_id = i;
        thread_data_array[i].total_threads = threads;
    }

    ksequence.open("thread/ksequence_S"+std::to_string(params.S)+"_p"+std::to_string(params.p)+".dat",std::ios::app);
    msequence.open("thread/msequence_S"+std::to_string(params.S)+"_p"+std::to_string(params.p)+".dat",std::ios::app);
    llength.open("thread/llength_S"+std::to_string(params.S)+"_p"+std::to_string(params.p)+".dat",std::ios::app);

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
    gp.ucm = pnet.get_ucm();

    gp.i_active_states = pnet.get_active_states();
    gp.i_active_r = pnet.get_active_r();
    gp.i_theta = pnet.get_theta();
    gp.i_inactive_r = pnet.get_inactive_r();
    gp.i_inactive_states = pnet.get_inactive_states();


    gp.stop = false;

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

    ksequence.close();
    msequence.close();
    llength.close();

    t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    std::cout << "DURATION: " << duration << std::endl;

}
