
#include <mpi.h>
#include "parameters_struct.h"
#include "parallel_scheduler.h"
#include <iostream>

#define GET_READY 1
#define EXIT_PROCESS 0

#ifdef _FLOAT_PRECISION
#define MPIFPV MPI::FLOAT
#elif _DOUBLE_PRECISION
#define MPIFPV MPI::DOUBLE
#endif

std::vector<parameters> PPS::plist;

void PPS::start(){

    //MPI OPEN
    /********************************************/
    MPI::Init();

    int comm_size =  MPI::COMM_WORLD.Get_size(); //Set global variable
    int pid = MPI::COMM_WORLD.Get_rank(); //Set global variable
    /********************************************/

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

    if(pid == 0){

        int i;
        int start = 1,ready;
        MPI::Status status;

        while(true){



            //Check first ready-to-compute process
            MPI::COMM_WORLD.Recv(&ready, 1, MPI::INT, MPI_ANY_SOURCE, 0, status);


            //Send a 0 status to all the process to stop
            if(PPS::plist.size() == 0 ){
                start = 0;
                MPI::COMM_WORLD.Send(&start, 1, MPI::INT, status.Get_source(), 0);
            }else{
                //Prepare him to receive the params and start the sim (a boolean, 0 exit 1 get ready)
                MPI::COMM_WORLD.Send(&start, 1, MPI::INT, status.Get_source(), 0);


                //Deploy the parameterer struct


                //Pullout the parameter struct from the list
            }

        }



    }else{

        bool status;

        struct parameters recvparams;
        while(true){
            status == 0;
            //Send with a point to point that you are free
            MPI::COMM_WORLD.Send(&status, 1, MPI::BOOL, 0, 0);

            //receive status value to exit or to receive a new params struct to start new sim

            if(status == GET_READY){
                //wait to receive parameters

                //Start sim

            }else if(status == EXIT_PROCESS){
                break;
            }

        }

    }


    MPIPPSTRUCT.Free();
    //MPI CLOSE
    /**************************************************************************/
    MPI::Finalize();
    /**************************************************************************/

}
