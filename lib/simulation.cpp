#include <iostream>
#include <random>
#include <string>

#include "pattern_gen.h"
#include "random_seq.h"
#include "hc_pnet.h"
#include "lc_pnet.h"
#include "parameters_struct.h"
#include "simulation.h"
#include "config.h"

void PottsSim(struct parameters params, std::string mode){

    std::string strategy;

    if(mode == "auto"){
        if( ((__fpv)params.N / params.C < 1.7) && (params.N > 1500) ){
            strategy = "hc";
        }else{
            strategy = "lc";
        }
    }

    //Random seed init
    std::default_random_engine generator;
    generator.seed(12345);

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
    generator.seed(12345);

    if(strategy == "hc"){
        HC_PNet pnet(params.N,
                    params.C,
                    params.S
                        );

        //Connect units
        pnet.connect_units(generator);
        pnet.init_network(params.beta,params.U,params.p,params.a,pgen.get_patt());

        /***************************************************************************
        DYNAMICS
        ***************************************************************************/
        //Start the dynamics
        pnet.start_dynamics(generator,
                            params.p,
                            params.tstatus, //tstatus (tempostampa)
                            params.nupdates,  //Number of updates
                            pgen.get_patt(),
                            0, //Pattern number
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
        std::cout << "HIGH CONNECTIVITY CHOSEN" << std::endl;
    }else if(strategy == "lc"){
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
        //Start the dynamics
        pnet.start_dynamics(generator,
                            params.p,
                            params.tstatus, //tstatus (tempostampa)
                            params.nupdates,  //Number of updates
                            pgen.get_patt(),
                            0, //Pattern number
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
        std::cout << "LOW CONNECTIVITY CHOSEN" << std::endl;
    }


}
