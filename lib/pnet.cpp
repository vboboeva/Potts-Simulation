#include "pnet.h"


void PNet::get_status(const int & p, const int & tx, const int & t, const int * xi, const __fpv & a, int & Mumaxold, int & Mumax, int & steps, bool & stop){

    __fpv m[p];
    //__fpv time = t*1.0/N*1.0;
    int mu;
    __fpv Mmax;

    evaluate_m(p,a,xi,m);

    if(t > (tx + 10 * N)){
        Mmax = -1.0;
        Mumax = p + 1;

        for(mu = 0; mu < p; ++mu){
            if(m[mu] > Mmax){
                Mmax = m[mu];
                Mumax = mu;
            }
        }

        if(Mumaxold != Mumax && Mmax > 0.5){
            steps++;
            Mumaxold = Mumax;
        }
    }
    for(mu = 0; mu < p; ++mu){

        stop = true;

        if(m[mu] > 0.02){
            stop = false;
            break;
        }
    }

}
