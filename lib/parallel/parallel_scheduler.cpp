#include <mpi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

#include "parameters_struct.h"
#include "parallel_scheduler.h"
#include "utils.h"
#include "pthread_simulation.h"
#include "simulation.h"
//#include "master.h"

#define EXIT_PROCESS -1

#ifdef _FLOAT_PRECISION
#define MPIFPV MPI::FLOAT
#elif _DOUBLE_PRECISION
#define MPIFPV MPI::DOUBLE
#endif


/*
Send/Receive queue
Check http://stackoverflow.com/questions/10850716/what-exactly-happens-when-we-use-mpi-send-receive-functions
*/

std::vector<parameters> PPS::plist;
int PPS::comm_size; //Set global variable
int PPS::pid; //Set global variable

void PPS::start(){

    //Define parameters struct for mpi
    //Refer to this as an example http://lists.mcs.anl.gov/pipermail/mpich-discuss/2009-April/004880.html
    MPI::Datatype MPIPPSTRUCT;
    int blockcounts[2];
    MPI::Aint offsets[2];
    MPI::Datatype datatypes[2];
    MPI::Aint extent,lb;

    blockcounts[0] = 10; //Number of ints
    blockcounts[1] = 13; //number of __fpv
    datatypes[0] = MPI::INT;
    datatypes[1] = MPIFPV;
    offsets[0] = 0;

    MPI::INT.Get_extent(lb, extent);

    offsets[1] = blockcounts[0] * extent;

    MPIPPSTRUCT = MPIPPSTRUCT.Create_struct(2,blockcounts,offsets, datatypes);
    MPIPPSTRUCT.Commit();

    //MASTER
    if(PPS::pid == 0){

        struct parameters temp;
        int start,i,countdown = PPS::comm_size-1;
        bool ready = false;
        MPI::Status status;

        //Logs
        std::ofstream logsfile;
        logsfile.open("tslogs.txt",  std::fstream::out | std::fstream::trunc);

        while(true){


            if(countdown == 0) break;

            //Check first ready-to-compute process
            MPI::COMM_WORLD.Recv(&ready, 1, MPI::BOOL, MPI_ANY_SOURCE, 0, status);

            //Logs
            logsfile << "Remaining sims: " << PPS::plist.size()  << " process countdown: " << countdown << std::endl;

            //Send a 0 status to all the process to stop
            if(ready){
                if(PPS::plist.size() == 0 ){
                    start = EXIT_PROCESS;
                    MPI::COMM_WORLD.Send(&start, 1, MPI::INT, status.Get_source(), 0);
                    countdown = countdown - 1;
                }else{
                    //Prepare him to receive the params and start the sim (an int that contains the simulation number (-1 = exit))
                    start = PPS::plist.size() - 1;
                    MPI::COMM_WORLD.Send(&start, 1, MPI::INT, status.Get_source(), 0);

                    temp = PPS::plist.back();

                    //temp.N = status.Get_source() * 10;

                    //Deploy the parameterer struct
                    MPI::COMM_WORLD.Send(&temp, 1, MPIPPSTRUCT, status.Get_source(), 0);

                    //Pullout the parameter struct from the list
                    plist.pop_back();
                }
            }
            ready = false;
        }

        logsfile.close();



    }else{
        //SLAVES
        int status;
        bool ready = true;
        struct parameters recvparams;

        while(true){
            status == EXIT_PROCESS;
            //Send with a point to point that you are free
            MPI::COMM_WORLD.Send(&ready, 1, MPI::BOOL, 0, 0);

            //receive status value to exit or to receive a new params struct to start new sim
            MPI::COMM_WORLD.Recv(&status, 1, MPI::INT, 0, 0);

            if(status != EXIT_PROCESS){
                //wait to receive parameters

                MPI::COMM_WORLD.Recv(&recvparams, 1, MPIPPSTRUCT, 0, 0);
                //Start sim
                //PottsSim(recvparams,"output/"+ std::to_string(PPS::pid) + "_", status);
                //PottsSim(recvparams);
                ThreadedPottsSim(recvparams,10,50);

            }else{
                std::cout << "I'm the process "<< PPS::pid << ", ready to die." << std::endl;
                break;
            }

        }

    }

    MPIPPSTRUCT.Free();

}
