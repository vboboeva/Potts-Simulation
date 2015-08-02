#ifndef __VLC_PNET_H
#define __VLC_PNET_H


#include "lc_pnet.h"
#include "config.h"


class VLC_PNet : public LC_PNet {
    private:

        void update_rule(const int & unit,
                         const int & pattern,
                         const __fpv & U,
                         const __fpv & w,
                         const __fpv & g,
                         const __fpv & tau,
                         const __fpv & b1,
                         const __fpv & b2,
                         const __fpv & b3,
                         const __fpv & beta,
                         const int & tx,
                         const int & t
                         );

    public:
        VLC_PNet(const int & N, const int & C, const int & S):LC_PNet(N,C,S){};
        ~VLC_PNet(){};

        void start_dynamics(const std::default_random_engine & generator,
                          const int & p,
                          const int & tstatus,
                          const int & nupdates,
                          const int * xi,
                          const int & pattern,
                          const __fpv & a,
                          const __fpv & U,
                          const __fpv & w,
                          const __fpv & g,
                          const __fpv & tau,
                          const __fpv & b1,
                          const __fpv & b2,
                          const __fpv & b3,
                          const __fpv & beta,
                          const int & tx
                          );

};

#endif
