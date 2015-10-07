

#include "parallel_scheduler.h"

#include <iostream>

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

    if(PPS::pid == 0){
        int i,nsim;

        if(argc == 1){
            std::cout << "Error, this program requires the number of simulation to launch as argument" << std::endl;
            MPI::COMM_WORLD.Abort(0);
            //MPI::Finalize();
            return 1;
        }
        nsim = std::atoi(argv[1]);

        t1 = MPI::Wtime();
        std::cout << "Potts simulation" << std::endl;
        struct parameters params;
        load_params("params.cfg", params);

        for(i = 0; i < nsim; ++i) PPS::plist.push_back(params);

    }



    PPS::start();

    if(PPS::pid == 0){
        t2 = MPI::Wtime();
        std::cout << "[NSIM] [ELAPSED TIME] " << std::atoi(argv[1]) << " " << t2-t1 << std::endl;
        std::cout << "End of the simulation" << std::endl;
    }

    //MPI CLOSE
    /**************************************************************************/
    MPI::Finalize();
    /**************************************************************************/


    return 0;
}
