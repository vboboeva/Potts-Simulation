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
PNetwork::PNetwork( int N, int S, int C, double beta, double U){

    int i;

    this->N = N;
    this->S = S;
    this->C = C;
    this->beta = beta;
    this->U = U;

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

    int i,j;
    std::default_random_engine generator;
    generator.seed(time(NULL));

    RandomSequence sequence(N);

    //Fill cm matrix with indices of potts units
    for(i=0; i<N; i++){
        sequence.shuffle(generator);
        for(j=0; j < C; ++j){
            std::memcpy(cm + C*i, sequence.begin(), C * sizeof(*sequence.begin()));
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

void PNetwork::Init_Units(){

    int i;

    //Generate connection matrix
    this->ConnectUnits();

    //Init unit states and r
    for(i=0; i < N; ++i){
        network[i]->init(beta,U);
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
