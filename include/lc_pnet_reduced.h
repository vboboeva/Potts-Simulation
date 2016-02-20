#ifndef __LC_PNET_REDUCED_H
#define __LC_PNET_REDUCED_H

#include <random>

#include "lc_pnet.h"

class LC_PNet_Reduced : public LC_PNet{
    protected:
    public:
        LC_PNet_Reduced(const int & N, const int & C, const int & S, int * cm, __fpv * J);

};

#endif
