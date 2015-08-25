#include <math.h>
#include <iostream>
#include <random>
#include <fstream>
#include <chrono>
#include <iomanip>

#include "hc_pnet.h"
#include "config.h"
#include "random_seq.h"


/*******************************************************************************
HIGH CONNECTIVITY POTTS NETWORK
*******************************************************************************/
HC_PNet::HC_PNet(const int & N, const int & C, const int & S) : N(N),C(C),S(S){

    int i,j;

    this->cm = new int[N * C];
    this->ucm = new int[N * N];
    this->J = new __fpv[N * S * N * S];
    this->inactive_states = new __fpv[N];
    this->active_states = new __fpv[N * S];
    this->inactive_r = new __fpv[N];
    this->active_r = new __fpv[N * S];
    this->h = new __fpv[N*S];
    this->theta = new __fpv[N*S];

    //Init to 0 "1-0 matrix"
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            ucm[i*N + j] = 0;
        }
    }

}

HC_PNet::~HC_PNet(){

    delete[] this->cm;
    delete[] this->ucm;
    delete[] this->J;
    delete[] this->inactive_states;
    delete[] this->active_states;
    delete[] this->inactive_r;
    delete[] this->active_r;
    delete[] this->h;
    delete[] this->theta;

}

void HC_PNet::connect_units(std::default_random_engine & generator){

    int i,j;
    RandomSequence sequence(this->N); //Sequence between 0 and N-1

    //Fill cm matrix with indices of potts units
    for(i = 0; i < this->N; ++i){
        //Shuffle the sequence
        sequence.shuffle(generator);

        //Store in the inverse cm
        for(j = 0; j < C; ++j){
            this->cm[C*i + j] = sequence.begin()[j];
            this->ucm[i*N + sequence.begin()[j]] = 1;
        }
    }

}



void HC_PNet::init_states(const __fpv & beta, const __fpv & U){

    int i,j;

    __fpv n = -2 * beta - 2 * exp(beta * U) - 2 * S+sqrt(pow(2 * beta + 2 * exp(beta * U)+2 * S,2)+8 * (-beta * beta - 2 * beta * S + 2 * beta *S * exp(beta * U)));
    __fpv d = 2 * (-beta * beta - 2 * beta * S + 2 * beta * S * exp(beta * U));

    for(i = 0; i < this->N; ++i){

        for(j = 0; j < this->S; ++j){
            this->active_states[i * S + j] = n / d;
        }

        this->inactive_states[i] = 1 - this->S*this->active_states[i * S];
        this->inactive_r[i] = 1 - this->inactive_states[i];
    }
}
void HC_PNet::init_J(const int & p, const __fpv & a, const int * xi){

    int i,j,k,l,m;
    __fpv as = a/S;

    //Generate Jkxl
    for(i = 0; i < N; ++i){
        for(j = 0; j < S; ++j){

            this->h[S*i + j] = 0;

            for(k = 0; k < N; ++k){
                for(l = 0; l < S; ++l){

                    this->J[S*N*S*i + N*S*j + S*k + l] = 0;

                    if(ucm[i*N + k] == 1){

                        for(m = 0; m < p; ++m){
                            this->J[S*N*S*i + N*S*j + S*k + l] += ((xi[p * i + m]==j)-as)*((xi[p * k + m]==l)-as);
                        }

                        this->J[S*N*S*i + N*S*j + S*k + l] /= a * (1 - as)* C;

                        this->h[S*i + j] += this->J[S*N*S*i + N*S*j + S*k + l] * this->active_states[S*k + l];
                    }

                }
            }

            this->active_r[S*i + j] = this->h[S*i + j];
            this->theta[S*i + j] = this->active_states[S*i + j];

        }
    }
}

void HC_PNet::init_network(const __fpv & beta, const __fpv & U, const int & p, const __fpv & a, const int * xi){

        //Init states
        this->init_states(beta,U);

        //Init J
        this->init_J(p,a,xi);


}

void HC_PNet::update_rule(const int & unit, const int & pattern, const __fpv & U, const __fpv & w, const __fpv & g, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const __fpv & beta, const int & tx, const int & t){
    //tx == n0 in the old code, "time 'x' "
    int i,j;
    __fpv self=0, INcost, rmax, Z;
    int tsize = this->N * this->S;

    rmax = this->inactive_r[unit];

    for(i = 0; i < this->S; ++i){
        self += this->active_states[unit*S + i];
        this->h[unit*S + i] = 0;
    }
    self = (w / this->S) * self;

    INcost = (t > tx) * g * exp(-(t-tx)/tau);

    for(i = 0; i < this->S; ++i){

        for(j = 0; j < tsize; ++j){
            this->h[unit*S + i] += this->J[S*N*S*unit + N*S*i + j] * this->active_states[j];
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

void HC_PNet::start_dynamics(std::default_random_engine & generator, const int & p,const int & tstatus, const int & nupdates, const int * xi, const int & pattern, const __fpv & a, const __fpv & U, const __fpv & w, const __fpv & g, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const __fpv & beta, const int & tx){

    //The code here is wrote for different cases defined during preprocessor
    int i,j,k,t;
    int unit;

    RandomSequence sequence(this->N);
    __fpv latching_length = 0;
    bool stop = false;
    int Mumax = p + 5, Mumaxold = p + 5, steps = 0;

    t = 0;
    //First loop = times the whole network has to be updated
    for(i = 0; i < nupdates; ++i){

        //Shuffle the random sequence
        #ifndef _NO_SHUFFLE
        sequence.shuffle(generator);
        #endif

        //Second loop = loop on all neurons serially
        for(j = 0; j < N; ++j){


            //unit = sequence.get(j);
            unit = j;

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
                latching_length = (double)t / N;
                this->get_status(p,tx,t,xi,a,Mumaxold,Mumax,steps,stop);
                if(stop &&  (t > tx + 100 * N)) goto end;
            }

            t++;

        }

    }
    end:

    if(t > tx + 100 * N){
        std::cout << "Latching length: " <<  latching_length << std::endl;
    }else{
        std::cout << "Simulation finished before reaching minimum steps" << std::endl;
    }

}


void HC_PNet::evaluate_m(const int & p, const __fpv & a, const int * xi, __fpv m[]){
    int i,j,k;

    __fpv ma, maa;
    __fpv invdenN = 1/(a*(1-a/S)*N);

    for(i = 0; i < p; ++i){
        maa = 0;
        for(j = 0; j < N; ++j){
            ma = 0;
            for(k = 0; k < S; ++k){
                ma += ( (xi[p * j + i] == k) - a/S) * this->active_states[S*j + k];
            }
            maa += ma;
        }
        m[i] = maa*invdenN;
    }
}

void HC_PNet::print_cm(){

    int i,j;

    for(i=0; i < this->N; i++){
        for(j=0; j < this->C; ++j){
            std::cout << this->cm[this->C*i + j]<< " ";
        }
        std::cout << std::endl;
    }

}

void HC_PNet::save_connections_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j;
    ofile.open(filename);

    for(i = 0; i < this->N; i++){
        for(j= 0; j < this->C; j++){
            ofile << *(this->cm + this->C*i + j)<< " ";
        }
        ofile << std::endl;
    }
    ofile.close();

}

void HC_PNet::save_states_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j;
    ofile.open(filename);
    ofile.precision(15);
    ofile << std::scientific;

    for(i = 0; i < this->N; i++){
        for(j= 0; j < this->S; j++){
            ofile << this->active_states[S*i + j]<< " ";
        }
        ofile << this->inactive_states[i]<< " ";
        ofile << std::endl;
    }
    ofile.close();

}

void HC_PNet::save_J_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j,k,l;
    ofile.open(filename);
    ofile.precision(15);
    ofile << std::scientific;

    for(i = 0; i < this->N; ++i){
        for(j = 0; j < this->S; ++j){
            for(k = 0; k < this->C; ++k){
                for(l = 0; l < this->S; ++l){
                    ofile << this->J[S*N*S*i + N*S*j + S*cm[C*i+k] + l] << " ";
                }
            }
        }
        ofile << std::endl;
    }

    ofile.close();
}
