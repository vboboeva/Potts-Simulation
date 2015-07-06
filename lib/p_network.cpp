#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <iostream>

#include "config.h"
#include "p_network.h"
#include "pattern_generation.h"
#include "p_unit.h"
#include "random_sequence.h"

/*******************************************************************************
POTTS NETWORK HANDLER
*******************************************************************************/
PNetwork::PNetwork(PatternGen & pgen, const int & C, const __fpv & U, const __fpv & w, const __fpv & g){

    int i,j;

    this->N = pgen.N;
    this->S = pgen.S;
    this->beta = pgen.beta;
    this->p = pgen.p;
    this->C = C;
    this->U = U;
    this->w = w;
    this->g = g;
    this->pgen = &pgen;

    this->network = new PUnit * [N];

    for(i = 0; i < N; ++i){
        this->network[i] = new PUnit(S,C,this->N);
    }

    this->cm = new int[N * C];
    this->ucm = new int[N * N];

    //Init to 0 "1-0 matrix"
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            this->ucm[i*N + j] = 0;
        }
    }

    this->m = new __fpv[pgen.p];
    this->states = new __fpv[this->N * this-> S];

}

PNetwork::~PNetwork(){

    int i;

    for(i = 0; i < N; ++i){
        delete this->network[i];
    }
    delete[] this->network;
    delete[] this->m;
    delete[] this->states;
    delete[] this->ucm;

}

void PNetwork::connect_units(){

    int i,j;
    RandomSequence sequence(N); //Sequence between 0 and N-1

    //Fill cm matrix with indices of potts units
    for(i = 0; i < N; ++i){
        //Shuffle the sequence
        sequence.shuffle(*this->pgen->generator);

        //Store in the inverse cm
        for(j = 0; j < C; ++j){
            cm[C*i + j] = sequence.begin()[j];
            ucm[i*N + sequence.begin()[j]] = 1;
        }
    }



}

void PNetwork::print_cm(){

    int i,j;

    for(i=0; i<N; i++){
        for(j=0; j < C; ++j){
            std::cout << cm[C*i + j]<< " ";
        }
        std::cout << std::endl;
    }

}

void PNetwork::evaluate_m(){

    int i,j,k;
    int * xi = this->pgen->get_patt();
    __fpv ma, maa;
    __fpv a = pgen->a;
    __fpv invdenN = 1/(a*(1-a/S)*N);

    for(i = 0; i < p; ++i){
        maa = 0;
        for(j = 0; j < N; ++j){
            ma = 0;
            for(k = 0; k < S; ++k){
                ma += ( (xi[p * j + i] == k) - a/S) * network[i]->get_state()[k];
            }
            maa += ma;
        }
        this->m[i] = maa*invdenN;
    }


}

void PNetwork::init_units(){

    int i,j;

    //Generate connection matrix
    this->connect_units();

    //Init unit states
    for(i=0; i < N; ++i){
        network[i]->init_states(this->beta,this->U);
    }

    //Fill the states array with the initialized states of the network
    //all except the "inactivity state"
    for(i=0; i < N; ++i){
        for(j = 0; j < S; ++j){
            this->states[i*S + j] = network[i]->get_state()[j];
        }
    }

    //Init J
    for(i=0; i < N; ++i){
        network[i]->init_J(p,pgen->a,this->pgen->get_patt(),i,this->ucm,this->network);
    }

    this->evaluate_m();

}

void PNetwork::save_states_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j;
    ofile.open(filename);

    for(i = 0; i < this->N; i++){
        for(j= 0; j < this->S+1; j++){
            ofile << *(this->network[i]->get_state()+j)<< " ";
        }
        ofile << std::endl;
    }
    ofile.close();

}
void PNetwork::save_J_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j,k,l;
    ofile.open(filename);

    for(i = 0; i < this->N; ++i){
        for(j = 0; j < this->S; ++j){
            for(k = 0; k < this->C; ++k){
                for(l = 0; l < this->S; ++l){
                    ofile << this->network[i]->get_cdata()[N*S*j + S*cm[C*i+k] + l] << " ";
                }
            }
        }
        ofile << std::endl;
    }

    ofile.close();
}

void PNetwork::save_connections_to_file(const std::string & filename){

    std::ofstream ofile;
    int i,j;
    ofile.open(filename);

    for(i = 0; i < this->N; i++){
        for(j= 0; j < this->C; j++){
            ofile << *(cm + C*i + j)<< " ";
        }
        ofile << std::endl;
    }
    ofile.close();

}

void PNetwork::start_dynamics(const int & nupdates, const int & tx, const __fpv & tau, const __fpv & b1, const __fpv & b2, const __fpv & b3, const int & pattern_number){
    //The code here is wrote for different cases defined during preprocessor

    int i,j,k,t;
    int unit;
    RandomSequence sequence(this->N);

    t = 0;
    //First loop = times the whole network has to be updated
    for(i = 0; i < nupdates; ++i){

        //Shuffle the random sequence
        #ifndef _TEST
        sequence.shuffle(*this->pgen->generator);
        #endif

        //Second loop = loop on all neurons serially
        for(j = 0; j < N; ++j){


            unit = sequence.get(j);

            //Update the unit
            this->network[unit]->update_rule(this->pgen->get_patt(j)[pattern_number],
                                            this->states,
                                            this->U,
                                            this->w,
                                            this->g,
                                            tau,
                                            b1,
                                            b2,
                                            b3,
                                            this->pgen->beta,
                                            tx,
                                            t
                                            );


            for(k = 0; k < S; ++k){
                states[S*unit + k] = this->network[unit]->get_state()[k];
            }
            t++;


        }

    }

}
