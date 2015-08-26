#include <math.h>
#include <iostream>
#include <random>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <sstream>
#include <chrono>

#include "lc_pnet.h"
#include "config.h"
#include "random_seq.h"


/*******************************************************************************
HIGH CONNECTIVITY POTTS NETWORK
*******************************************************************************/
LC_PNet::LC_PNet(const int & N, const int & C, const int & S) : N(N),C(C),S(S){

    int i,j;

    this->cm = new int[N * C];
    this->ucm = new int[N * N];
    this->J = new __fpv[N * S * C * S]; //!"£"
    this->states = new __fpv*[N];
    this->r = new __fpv*[N];
    this->h = new __fpv[N*S];
    this->theta = new __fpv[N*S];

    //Init r and states
    for(i = 0; i < N; ++i){
        this->states[i] = new __fpv[S+1];
    }
    for(i = 0; i < N; ++i){
        this->r[i] = new __fpv[S+1];
    }

    //Init to 0 "1-0 matrix"
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            ucm[i*N + j] = 0;
        }
    }

}

LC_PNet::~LC_PNet(){

    int i;

    delete[] this->cm;
    delete[] this->ucm;
    delete[] this->J;
    delete[] this->h;
    delete[] this->theta;


    for(i = 0; i < N; ++i){
        delete[] this->states[i];
    }
    delete[] this->states;

    for(i = 0; i < N; ++i){
        delete[] this->r[i];
    }
    delete[] this->r;

}

void LC_PNet::connect_units(std::default_random_engine & generator){

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



void LC_PNet::init_states(const __fpv & beta, const __fpv & U){

    int i,j;

    __fpv n = -2 * beta - 2 * exp(beta * U) - 2 * S+sqrt(pow(2 * beta + 2 * exp(beta * U)+2 * S,2)+8 * (-beta * beta - 2 * beta * S + 2 * beta *S * exp(beta * U)));
    __fpv d = 2 * (-beta * beta - 2 * beta * S + 2 * beta * S * exp(beta * U));

    for(i = 0; i < this->N; ++i){

        for(j = 0; j < this->S; ++j){
            this->states[i][j] = n / d;
        }

        this->states[i][S] = 1 - this->S*this->states[i][0];
        this->r[i][S] = 1 - this->states[i][S];
    }
}
void LC_PNet::init_J(const int & p, const __fpv & a, const int * xi){

    int i,j,k,l,m;
    __fpv as = a/S;

    //Generate Jkxl
    for(i = 0; i < N; ++i){
        for(j = 0; j < S; ++j){

            this->h[S*i + j] = 0;

            for(k = 0; k < C; ++k){
                for(l = 0; l < S; ++l){

                    this->J[S*C*S*i + C*S*j + S*k + l] = 0;

                    for(m = 0; m < p; ++m){
                        this->J[S*C*S*i + C*S*j + S*k + l] += ((xi[p * i + m]==j)-as)*((xi[p * cm[C*i + k] + m]==l)-as);
                    }

                    this->J[S*C*S*i + C*S*j + S*k + l] /= (__fpv)(a * (1.0 - as)*(__fpv)C);

                    this->h[S*i + j] += this->J[S*C*S*i + C*S*j + S*k + l] * this->states[cm[C*i + k]][l];

                }
            }

            this->r[i][j] = this->h[S*i + j];
            this->theta[S*i + j] = this->states[i][j];

        }
    }


}

void LC_PNet::init_network(const __fpv & beta, const __fpv & U, const int & p, const __fpv & a, const int * xi){

        //Init states
        this->init_states(beta,U);

        //Init J
        this->init_J(p,a,xi);


}

void LC_PNet::update_rule(const int & unit, const __fpv buffer[], const int & pattern, const __fpv & U, const __fpv & w, const __fpv & g, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const __fpv & beta, const int & tx, const int & t){

    // //tx == n0 in the old code, "time 'x' "
    // int i,j;
    // __fpv self=0, INcost, rmax, Z;
    // int tsize = this->C * this->S;
    //
    // rmax = this->inactive_r[unit];
    //
    // for(i = 0; i < this->S; ++i){
    //     self += this->active_states[unit*S + i];
    //     this->h[unit*S + i] = 0; //Maybe h can be even a small array of size S, since it's erased at each update
    // }
    //
    // self = (w / this->S) * self;
    //
    // INcost = (t > tx) * g * exp(-(t-tx)/tau);
    //
    // for(i = 0; i < this->S; ++i){
    //
    //     //Inside here maybe different order of + and * so slightly different solutions, have to check.
    //     for(j = 0; j < tsize; ++j){
    //         this->h[unit*S + i] += this->J[S*C*S*unit + C*S*i + j] * buffer[j];
    //     }
    //
    //     this->h[unit*S + i] += w * this->active_states[unit*S + i] - self + INcost * (pattern == i);
    //
    //     this->theta[unit*S + i] += b2 * (this->active_states[unit*S + i]-this->theta[unit*S + i]);
	//     this->active_r[unit*S + i] += b1 * (this->h[unit*S + i]-this->theta[unit*S + i]-this->active_r[unit*S + i]);
    //
    //     rmax = this->active_r[unit*S + i] * (this->active_r[unit*S + i] > rmax) - ((this->active_r[unit*S + i] > rmax) - 1) * rmax;
    //
    //     /*
    //     if(this->active_r[unit*S + i]>rmax){
    //         rmax=this->active_r[unit*S + i];
	//     }
    //     */
    //
    //
    // }
    //
    // this->inactive_r[unit] += b3 * (1.0 - this->inactive_states[unit] - this->inactive_r[unit]);
    //
    // Z=0;
    //
    // for(i = 0; i < S; ++i){
    //     Z += exp(beta * (this->active_r[unit*S + i] - rmax));
    // }
    //
    // Z += exp(beta * (this->inactive_r[unit] + U - rmax));
    //
    //
    // for(i = 0; i < S; ++i){
    // 	this->active_states[unit*S + i] = exp(beta * (this->active_r[unit*S + i] - rmax)) / Z;
    // }
    //
    // this->inactive_states[unit]=exp(beta * (this->inactive_r[unit] - rmax + U)) / Z;


}

void LC_PNet::start_dynamics(std::default_random_engine & generator, const int & p,const int & tstatus, const int & nupdates, const int * xi, const int & pattern, const __fpv & a, const __fpv & U, const __fpv & w, const __fpv & g, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const __fpv & beta, const int & tx){

    //The code here is wrote for different cases defined during preprocessor
    int i,j,k,n,t;
    int unit;
    __fpv buffer[C * S];
    RandomSequence sequence(this->N);

    __fpv latching_length = 0;
    bool stop = false;
    int Mumax = p + 5, Mumaxold = p + 5, steps = 0;

    int l,m,ts;
    __fpv self=0, INcost, rmax, Z;
    int tsize = this->C * this->S;
    const __fpv tb3 = b3;

    t = 0;

    //First loop = times the whole network has to be updated
    for(i = 0; i < nupdates; ++i){

        //Shuffle the random sequence
        #ifndef _NO_SHUFFLE
        sequence.shuffle(generator);
        #endif

        //Second loop = loop on all neurons serially
        for(j = 0; j < N; ++j){

            unit = sequence.get(j);

            for(k = 0; k < this->C; ++k){
                for(n = 0; n < this->S; ++n){
                    buffer[k*S + n] = this->states[cm[unit * C + k]][n];
                }
            }


            /*
            //Update the unit
            this->update_rule(unit,
                             buffer,
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
            */
            ////////////////////////////////////////////////////////////////////
            //tx == n0 in the old code, "time 'x' "
            rmax = this->r[unit][S];
            self = 0;

            for(l = 0; l < this->S; ++l){
                self += this->states[unit][l];
                this->h[unit*S + l] = 0; //Maybe h can be even a small array of size S, since it's erased at each update
            }

            self = (w / this->S) * self;

            INcost = (t > tx) * g * exp(-(t-tx)/tau);

            for(l = 0; l < this->S; ++l){

                //Inside here maybe different order of + and * so slightly different solutions, have to check.
                for(m = 0; m < tsize; ++m){
                    this->h[unit*S + l] += this->J[S*C*S*unit + C*S*l + m] * buffer[m];
                }


                this->h[unit*S + l] += w * this->states[unit][l] - self + INcost * (xi[p * unit + pattern] == l);

                this->theta[unit*S + l] += b2 * (this->states[unit][l]-this->theta[unit*S + l]);
                this->r[unit][l] += b1 * (this->h[unit*S + l]-this->theta[unit*S + l]-this->r[unit][l]);

                rmax = this->r[unit][l] * (this->r[unit][l] > rmax) - ((this->r[unit][l] > rmax) - 1) * rmax;

                /*
                if(this->active_r[ts + i]>rmax){
                    rmax=this->active_r[ts + i];
                }
                */


            }

            this->r[unit][S] += tb3*(1.0 - this->states[unit][S] - this->r[unit][S]);

            Z=0;

            for(l = 0; l < S; ++l){
                Z += exp(beta * (this->r[unit][l] - rmax));
            }

            Z += exp(beta * (this->r[unit][S] + U - rmax));


            for(l = 0; l < S; ++l){
                this->states[unit][l] = exp(beta * (this->r[unit][l] - rmax)) / Z;
            }

            this->states[unit][S]=exp(beta * (this->r[unit][S] - rmax + U)) / Z;


            ////////////////////////////////////////////////////////////////////

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
        std::cout << " Latching length: " <<  latching_length << std::endl;
    }else{
        std::cout << "Simulation finished before reaching minimum steps" << std::endl;
    }


}

void LC_PNet::evaluate_m(const int & p, const __fpv & a, const int * xi, __fpv m[]){
    int i,j,k;

    __fpv ma, maa;
    __fpv invdenN = 1/(a*(1-a/S)*N);

    for(i = 0; i < p; ++i){
        maa = 0;
        for(j = 0; j < N; ++j){
            ma = 0;
            for(k = 0; k < S; ++k){
                ma += ( (xi[p * j + i] == k) - a/S) * this->states[j][k];
            }
            maa += ma;
        }
        m[i] = maa*invdenN;

    }

}

void LC_PNet::print_cm(){

    int i,j;

    for(i=0; i < this->N; i++){
        for(j=0; j < this->C; ++j){
            std::cout << this->cm[this->C*i + j]<< " ";
        }
        std::cout << std::endl;
    }

}

void LC_PNet::save_connections_to_file(const std::string & filename){

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

void LC_PNet::save_states_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j;
    ofile.open(filename);
    ofile.precision(15);
    ofile << std::scientific;

    for(i = 0; i < this->N; i++){
        for(j= 0; j < this->S; j++){
            ofile << this->states[i][j]<< " ";
        }
        ofile << this->states[i][S]<< " ";
        ofile << std::endl;
    }
    ofile.close();

}

void LC_PNet::save_J_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j,k,l;
    ofile.open(filename);
    ofile.precision(15);
    ofile << std::scientific;

    for(i = 0; i < this->N; ++i){
        for(j = 0; j < this->S; ++j){
            for(k = 0; k < this->C; ++k){
                for(l = 0; l < this->S; ++l){
                    ofile << this->J[S*C*S*i + C*S*j + S*k + l] << " ";
                }
            }
        }
        ofile << std::endl;
    }

    ofile.close();
}

void LC_PNet::import_connections(const std::string & filename){

    int i, j;
    std::ifstream ifile;
    ifile.open(filename);

    for(i = 0; i < this->N; ++i){
    	for(j = 0; j < this->C; ++j){
    		ifile >> this->cm[this->C*i + j];
    	}

    }

    ifile.close();
}
