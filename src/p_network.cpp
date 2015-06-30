#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <iostream>

#ifdef _BENCH
#include <chrono>
#endif

#include "p_network.h"
#include "pattern_generation.h"
#include "p_unit.h"
#include "random_sequence.h"

/*******************************************************************************
POTTS NETWORK HANDLER
*******************************************************************************/
PNetwork::PNetwork(PatternGen & pgen, int C, double U, double w, double g){

    int i;

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
        this->network[i] = new PUnit(S,C);
    }

    this->cm = new int[N * C];
    this->icm = new std::vector<uindx>[N];
    this->m = new double[pgen.p];
}

PNetwork::~PNetwork(){

    int i;

    for(i = 0; i < N; ++i){
        delete this->network[i];
    }
    delete[] this->network;
    delete[] this->icm;
    delete[] this->m;

}

void PNetwork::connect_units(){

    int i,j;
    struct uindx a;
    RandomSequence sequence(N); //Sequence between 0 and N-1

    //Fill cm matrix with indices of potts units
    for(i = 0; i < N; ++i){
        //Shuffle the sequence
        sequence.shuffle(*this->pgen->generator);

        //Copy the first C numbers
        //std::memcpy(cm + C*i, sequence.begin(), C * sizeof(*sequence.begin()));

        //Store in the inverse cm
        a.unit = i;
        for(j = 0; j < C; ++j){
            cm[C*i + j] = sequence.begin()[j];
            a.idx = j;
            this->icm[sequence.begin()[j]].push_back(a);
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
    double ma, maa;
    double a = pgen->a;
    double invdenN = 1/(a*(1-a/S)*N);

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

    int i;

    //Generate connection matrix
    this->connect_units();

    //Init unit states
    for(i=0; i < N; ++i){
        network[i]->init_states(this->beta,this->U);
    }

    //Init J
    for(i=0; i < N; ++i){
        network[i]->init_J(p,pgen->a,this->pgen->get_patt(),i,this->cm,this->network);
    }

    this->evaluate_m();


}

void PNetwork::save_states_to_file(std::string filename){

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
void PNetwork::save_J_to_file(std::string filename){

    std::ofstream ofile;
    int i,j,k,l;
    ofile.open(filename);

    for(i = 0; i < this->N; ++i){
        for(j = 0; j < this->S; ++j){
            for(k = 0; k < this->C; ++k){
                for(l = 0; l < this->S; ++l){
                    ofile << this->network[i]->get_cdata()[C*S*j + S*k + l] << " ";
                }
            }
        }
        ofile << std::endl;
    }

    ofile.close();
}

void PNetwork::save_connections_to_file(std::string filename){

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

void PNetwork::start_dynamics(const int nupdates, const int tx, const double tau, const double b1, const double b2, const double b3, const int pattern_number){
    //The code here is wrote for different cases defined during preprocessor


    #ifdef _TEST
    /***************************************************************************
    * FOR THE TEST SUITE
    ***************************************************************************/
    int i,j,t;
    double * new_states;
    RandomSequence sequence(this->N);
    std::vector<uindx>::iterator it;

    t = 0;
    //First loop = times the whole network has to be updated

    for(i = 0; i < nupdates; ++i){

        //Second loop = loop on all neurons serially
        for(j = 0; j < this->N; ++j){

            //Update the unit
            this->network[j]->update_rule(this->pgen->get_patt(j)[pattern_number],
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

            //Update the network with new numbers (this could be done in parallel)
            new_states = this->network[j]->get_state();

            for(it = this->icm[j].begin(); it != this->icm[j].end(); ++it){
                this->network[it->unit]->update_cdata(new_states,it->idx);
            }
            t++;

        }
    }

    #else
    /***************************************************************************
    * NORMAL RUN
    ***************************************************************************/
    int i,j,t;
    double * new_states;
    RandomSequence sequence(this->N);
    std::vector<uindx>::iterator it;

    #ifdef _BENCH
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    auto duration;

    t1 = high_resolution_clock::now();
    #endif

    t = 0;
    //First loop = times the whole network has to be updated

    for(i = 0; i < nupdates; ++i){



        //Shuffle the random sequence
        sequence.shuffle(*this->pgen->generator);

        //Second loop = loop on all neurons serially
        for(j = 0; j < this->N; ++j){

            //Update the unit
            this->network[sequence.get(j)]->update_rule(this->pgen->get_patt(j)[pattern_number],
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

            //Update the network with new numbers (this could be done in parallel)
            new_states = this->network[j]->get_state();

            for(it = this->icm[j].begin(); it != this->icm[j].end(); ++it){
                this->network[it->unit]->update_cdata(new_states,it->idx);
            }
            t++;

        }
    }

    #ifdef _BENCH
    t2 = high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    std::cout << "TOTAL UPDATE ELAPSED TIME(ms): "<< duration << std::endl;
    #endif

    #endif


}
