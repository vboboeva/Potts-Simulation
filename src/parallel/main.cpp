

#include "parallel_scheduler.h"

#include <iostream>
#include <random>
#include "utils.h"
#include "simulation.h"
#include "parameters_struct.h"

int main(int argc, char *argv[]){

    //MPI OPEN
    /********************************************/
    MPI::Init();

    PPS::comm_size =  MPI::COMM_WORLD.Get_size(); //Set global variable
    PPS::pid = MPI::COMM_WORLD.Get_rank(); //Set global variable
    /********************************************/

    double t1,t2;
    int sims; //count the total sims

    if(PPS::pid == 0){
        int i,j;

        t1 = MPI::Wtime();
        std::cout << "Potts simulation" << std::endl;
        struct parameters params;
        load_params("params.cfg", params);

        int S[] = {3, 5, 7, 9};
        for(i = 0; i < 4; ++i){
            params.S = S[i];
            for(j = 10; j < 1000; j+=50 ){
                params.p = j;
                PPS::plist.push_back(params);
            }
        }

        sims = PPS::plist.size();

    }



    PPS::start();

    if(PPS::pid == 0){
        t2 = MPI::Wtime();
        std::cout << "[NSIM] [ELAPSED TIME] " << sims << " " << t2-t1 << std::endl;
        std::cout << "End of the simulation" << std::endl;
    }

    //MPI CLOSE
    /**************************************************************************/
    MPI::Finalize();
    /**************************************************************************/


    return 0;
}
