
#include <mpi.h>
#include "parameters_struct.h"
#include "parallel_scheduler.h"
#include <iostream>
#include "utils.h"

#define GET_READY 1
#define EXIT_PROCESS 0

#ifdef _FLOAT_PRECISION
#define MPIFPV MPI::FLOAT
#elif _DOUBLE_PRECISION
#define MPIFPV MPI::DOUBLE
#endif

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

    blockcounts[0] = 9; //Number of ints
    blockcounts[1] = 13; //number of __fpv
    datatypes[0] = MPI::INT;
    datatypes[1] = MPIFPV;
    offsets[0] = 0;

    MPI::INT.Get_extent(lb, extent);

    offsets[1] = blockcounts[0] * extent;

    MPIPPSTRUCT = MPIPPSTRUCT.Create_struct(2,blockcounts,offsets, datatypes);
    MPIPPSTRUCT.Commit();

    if(PPS::pid == 0){


        std::cout << "SAY HI: "<< PPS::pid << std::endl;

        struct parameters temp;
        int i,countdown = PPS::comm_size-1;
        bool start = GET_READY,ready;
        MPI::Status status;

        while(true){


            if(countdown == 0) break;

            //Check first ready-to-compute process
            MPI::COMM_WORLD.Recv(&ready, 1, MPI::BOOL, MPI_ANY_SOURCE, 0, status);


            //Send a 0 status to all the process to stop
            if(ready){
                if(PPS::plist.size() == 0 ){
                    start = EXIT_PROCESS;
                    MPI::COMM_WORLD.Send(&start, 1, MPI::BOOL, status.Get_source(), 0);
                    countdown = countdown - 1;
                }else{
                    //Prepare him to receive the params and start the sim (a boolean, 0 exit 1 get ready)
                    MPI::COMM_WORLD.Send(&start, 1, MPI::BOOL, status.Get_source(), 0);

                    temp = PPS::plist.back();

                    //Deploy the parameterer struct
                    MPI::COMM_WORLD.Send(&temp, 1, MPIPPSTRUCT, status.Get_source(), 0);

                    //Pullout the parameter struct from the list
                    plist.pop_back();
                }
            }
            ready = false;
        }



    }else{


        bool status = false, ready = true;

        struct parameters recvparams;
        while(true){
            status == EXIT_PROCESS;
            //Send with a point to point that you are free
            MPI::COMM_WORLD.Send(&ready, 1, MPI::BOOL, 0, 0);

            //receive status value to exit or to receive a new params struct to start new sim
            MPI::COMM_WORLD.Recv(&status, 1, MPI::BOOL, 0, 0);

            if(status == GET_READY){
                //wait to receive parameters
                MPI::COMM_WORLD.Recv(&recvparams, 1, MPIPPSTRUCT, 0, 0);
                //Start sim
                std::cout << "//////////////////////////////////////////////////////////////////////////////////"<< std::endl;
                std::cout << "SAY HI: "<< PPS::pid << std::endl;
                print_params(recvparams);
                std::cout << "//////////////////////////////////////////////////////////////////////////////////"<< std::endl;

            }else if(status == EXIT_PROCESS){
                break;
            }

        }

    }

    MPIPPSTRUCT.Free();


}
