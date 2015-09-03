#include <mpi.h>
#include "config.h"
#include "parameters_struct.h"
#include <vector>

class PPS{
    public:
        static std::vector<parameters> plist;
        static void start();

};
