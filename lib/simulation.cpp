#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <time.h>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "parameters_struct.h"
#include "simulation.h"
#include "config.h"

void PottsSim(struct parameters params, const int & pid, const int & msml){

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

    t1 = std::chrono::high_resolution_clock::now();

    std::ofstream ksequence;
    std::ofstream msequence;
    std::ofstream llength;


    //Random seed init
    std::default_random_engine generator;
    //generator.seed(53434321); Inside pattern gen I'm using srand48

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

    int patt,i;
    int p_cued = params.p;
    //If the number of pattern is higher than 100 simply run the sim for 100 different cues
    if(params.p >= 50) p_cued = 50;

    std::vector<bool> llseq;
    std::vector<bool>::iterator l;
    std::vector<int>::iterator k;
    std::vector<__fpv>::iterator m;

    ksequence.open("serial/ksequence_S"+std::to_string(params.S)+"_p"+std::to_string(params.p)+".dat",std::ios::app);
    msequence.open("serial/msequence_S"+std::to_string(params.S)+"_p"+std::to_string(params.p)+".dat",std::ios::app);
    llength.open("serial/llength_S"+std::to_string(params.S)+"_p"+std::to_string(params.p)+".dat",std::ios::app);




    for(patt=0; patt < p_cued ; patt++){

        std::cout << "S: "<< params.S << " p: "<< params.p << " cued: " << patt << std::endl;
        generator.seed(12345);
        pnet.reset(params.beta,params.U);


        pnet.start_dynamics(generator,
                            params.p,
                            params.tstatus, //tstatus (tempostampa)
                            params.nupdates,  //Number of updates
                            pgen.get_patt(),
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

        ksequence << patt << " ";
        k = pnet.ksequence.begin();
        while( k!= pnet.ksequence.end()){
            ksequence << *k << " ";
            k++;
        }
        ksequence << std::endl;

        msequence << patt << " ";
        m = pnet.msequence.begin();
        while( m!= pnet.msequence.end()){
            msequence << *m << " ";
            m++;
        }
        msequence << std::endl;

        llength << patt << " ";
        llength << pnet.latching_length << std::endl;

        if(pnet.infinite)llseq.push_back(pnet.infinite);
        if(llseq.size() == msml)break;

    }

    ksequence.close();
    msequence.close();
    llength.close();

    t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    std::cout << "DURATION: " << duration << std::endl;

}
