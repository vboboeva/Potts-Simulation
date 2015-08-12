#include <iostream>

#include "vlc_pnet.h"
#include "config.h"
#include "random_seq.h"

void VLC_PNet::update_rule(const int & unit, const int & pattern, const __fpv & U, const __fpv & w, const __fpv & g, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const __fpv & beta, const int & tx, const int & t){
    //tx == n0 in the old code, "time 'x' "
    int i,j,k;
    __fpv self=0, INcost, rmax, Z;
    int tsize = this->C * this->S;

    rmax = this->inactive_r[unit];

    for(i = 0; i < this->S; ++i){
        self += this->active_states[unit*S + i];
        this->h[unit*S + i] = 0;
    }
    self = (w / this->S) * self;

    INcost = (t > tx) * g * exp(-(t-tx)/tau);

    for(i = 0; i < this->S; ++i){

        for(j = 0; j < C; ++j){
            for(k = 0; k < S; ++k){
                this->h[unit*S + i] += this->J[S*C*S*unit + C*S*i + S*j + k] * this->active_states[S*cm[unit * C + j] + k];
            }
        }

        this->h[unit*S + i] += w * this->active_states[unit*S + i] - self + INcost * (pattern == i);

        this->theta[unit*S + i] += b2 * (this->active_states[unit*S + i]-this->theta[unit*S + i]);
	    this->active_r[unit*S + i] += b1 * (this->h[unit*S + i]-this->theta[unit*S + i]-this->active_r[unit*S + i]);

        rmax = this->active_r[unit*S + i] * (this->active_r[unit*S + i] > rmax) - ((this->active_r[unit*S + i] > rmax) - 1) * rmax;
        /*
        if(this->active_r[unit*S + i]>rmax){
            rmax=this->active_r[unit*S + i];
	    }
        */

    }

    this->inactive_r[unit] += b3 * (1 - this->inactive_states[unit] - this->inactive_r[unit]);

    Z=0;

    for(i = 0; i < S; ++i){
        Z += exp(beta * (this->active_r[unit*S + i] - rmax));
    }

    Z += exp(beta * (this->inactive_r[unit] + U - rmax));

    for(i = 0; i < S; ++i){
    	this->active_states[unit*S + i] = exp(beta * (this->active_r[unit*S + i] - rmax)) / Z;
    }

    this->inactive_states[unit]=exp(beta * (this->inactive_r[unit] - rmax + U)) / Z;

}

void VLC_PNet::start_dynamics(std::default_random_engine & generator, const int & p,const int & tstatus, const int & nupdates, const int * xi, const int & pattern, const __fpv & a, const __fpv & U, const __fpv & w, const __fpv & g, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const __fpv & beta, const int & tx){

    //The code here is wrote for different cases defined during preprocessor
    int i,j,k,n,t;
    int unit;

    RandomSequence sequence(this->N);

    __fpv latching_length;
    bool stop = false;
    int Mumax = p + 5, Mumaxold = p + 5, steps = 0;

    t = 0;

    //First loop = times the whole network has to be updated
    for(i = 0; i < nupdates; ++i){

        //Shuffle the random sequence
        #ifndef _TEST
        sequence.shuffle(generator);
        #endif

        //Second loop = loop on all neurons serially
        for(j = 0; j < N; ++j){


            unit = sequence.get(j);

            //Update the unit
            this->update_rule(unit,
                             xi[p * unit + pattern],
                             U,
                             w,
                             g,
                             tau,
                             b1,
                             b2,
                             b3,
                             beta,
                             tx,
                             t
                             );

            if((t % tstatus) == 0){
                this->get_status(p,tx,t,xi,a,Mumaxold,Mumax,steps,stop);
                if(stop &&  (t > tx + 100 * N)) goto end;
            }

            t++;

        }

    }
    end:

    if(t > tx + 100 * N){
        latching_length = t*1.0 / N*1.0;
        std::cout << "Latching length: " <<  latching_length << std::endl;
    }else{
        std::cout << "Simulation finished before reaching minimum steps" << std::endl;
    }


}
