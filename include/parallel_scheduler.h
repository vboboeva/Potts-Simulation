#include <mpi.h>
#include "config.h"
#include "parameters_struct.h"
#include <vector>

class PPS{
    public:
        static int pid;
        static int comm_size;
        static std::vector<parameters> plist;
        static void start();

};
