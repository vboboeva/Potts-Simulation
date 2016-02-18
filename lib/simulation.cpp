#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "parameters_struct.h"
#include "simulation.h"
#include "config.h"

void PottsSim(struct parameters params, std::string filename, const int & id, std::string mode){

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::string strategy;
    std::ofstream ofile;
    //std::ostream & ofile = std::cout;
    ofile.open(filename, std::ios::app);

    // if(mode == "auto"){
    //     if( ((__fpv)params.N / params.C < 1.7) && (params.N > 1500) ){
    //         strategy = "hc";
    //     }else{
    //         strategy = "lc";
    //     }
    // }


    t1 = std::chrono::high_resolution_clock::now();
    //Random seed init
    std::default_random_engine generator;
    //generator.seed(12345);

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
    //Create the network
    //generator.seed(12345);


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
    t1 = std::chrono::high_resolution_clock::now();

    int patt_cued;

    std::vector<int>::iterator k;
    std::vector<__fpv>::iterator m;

    ofile << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"<< std::endl;
    for(patt_cued=0; patt_cued<params.p;patt_cued++){

        pnet.init_states(params.beta,params.U);

        pnet.ksequence.clear();
        pnet.msequence.clear();

        pnet.start_dynamics(generator,
                            params.p,
                            params.tstatus, //tstatus (tempostampa)
                            params.nupdates,  //Number of updates
                            pgen.get_patt(),
                            patt_cued, //Pattern cued
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

        k = pnet.ksequence.begin();
        ofile << std::setw(30) << std::left << "KSEQUENCE: ";
        while( k!= pnet.ksequence.end()){
            ofile << *k << " ";
            k++;
        }
        ofile << std::endl;

        m = pnet.msequence.begin();
        ofile << std::setw(30) << std::left << "MSEQUENCE: ";
        while( m!= pnet.msequence.end()){
            ofile << *m << " ";
            m++;
        }
        ofile << std::endl;
        //pnet.ksequence.get(i)
        ofile << std::setw(30) << std::left << "LATCHING LENGTH: "<< pnet.latching_length << std::endl;
        ofile << "---------------------------------------------------------------------------------"<< std::endl;

    }

    t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    ofile << std::setw(30) << std::left << "SIM #" << id << std::endl;
    ofile << "---------------------------------------------------------------------------------"<< std::endl;
    ofile << std::setw(30) << std::left << "SIM ELAPSED TIME(ms): "<< duration << std::endl;
    ofile << "---------------------------------------------------------------------------------"<< std::endl;
    ofile << "//////////////////////////////////////////////////////////////////////////////////"<< std::endl;
    ofile << std::endl;

    std::cout << "DURATION: " << duration << std::endl;


    ofile.close();
}
