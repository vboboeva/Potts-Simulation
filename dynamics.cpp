#include "main.h"

/*******************************************************************************
RANDOM SEQUENCE
*******************************************************************************/
RandomSequence::RandomSequence( int N){

    this->N = N;
    this->sequence = new int[N];

    int i;
    for(i = 0; i < N ; i++){
        sequence[i] = i;
    }

}

RandomSequence::~RandomSequence(){
    delete[] this->sequence;
}

int * RandomSequence::begin(){
    return this->sequence;
}

int * RandomSequence::end(){
    return &(this->sequence[this->N]);
}

void RandomSequence::shuffle(std::default_random_engine & generator){
    std::shuffle(this->begin(), this->end(), generator);
}

void RandomSequence::print(){

    int i;

    for(i = 0; i < this->N ; i++){
        std::cout << sequence[i] << " ";
    }

    std::cout << std::endl;
}

/*******************************************************************************
POTTS NETWORK HANDLER
*******************************************************************************/
PNetwork::PNetwork( int N, int S, int C){

    int i;

    this->N = N;
    this->S = S;
    this->C = C;

    this->network = new PUnit * [N];

    for(i = 0; i < N; ++i){
        this->network[i] = new PUnit(S,C);
    }

    this->cm = new int[N * C];
}

PNetwork::~PNetwork(){

    int i;

    for(i = 0; i < N; ++i){
        delete this->network[i];
    }

    delete[] this->network;

}

void PNetwork::ConnectUnits(){

    for(i=0; i<N; i++){
        i_c = 0;
        while(i_c<Cm)
        {
            new_one = 1;

            j = (int)((double)N*drand48());
            if(j==i) new_one = 0;
            for(x=0; x<i_c; x++){
                if(C[i][x]==j) new_one = 0;
            }

            if(new_one){
                C[i][i_c] = j;
                i_c++;
            }
        }
    }
}

/*******************************************************************************
POTTS UNIT
*******************************************************************************/
PUnit::PUnit( int S, int C){
    this->state = new double[S + 1];
    this->cdata = new double[2 * S * C * S];
    this->r = new double[S + 1];
    this->S = S;
    this->C = C;
}

PUnit::~PUnit(){

    delete[] this->state;
    delete[] this->cdata;
}

void PUnit::init( double beta, double U){

    int i;

    for(i = 0; i < S; ++i){
		state[i]=(-2*beta-2*exp(beta*U)-2*S+sqrt(pow(2*beta+2*exp(beta*U)+2*S,2)+8*(-beta*beta-2*beta*S+2*beta*S*exp(beta*U))))/(2*(-beta*beta-2*beta*S+2*beta*S*exp(beta*U)));
	}

	state[S] = 1 - S*state[0];
	r[S] = 1 - state[S];
}
