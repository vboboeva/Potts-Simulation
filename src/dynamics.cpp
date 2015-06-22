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
PNetwork::PNetwork(PatternGen & pgen, int C, double U){

    int i;

    this->N = pgen.N;
    this->S = pgen.S;
    this->beta = pgen.beta;
    this->p = pgen.p;
    this->C = C;
    this->U = U;
    this->pgen = &pgen;

    this->network = new PUnit * [N];

    for(i = 0; i < N; ++i){
        this->network[i] = new PUnit(S,C);
    }

    this->cm = new int[N * C];

    this->m = new double[pgen.p];
}

PNetwork::~PNetwork(){

    int i;

    for(i = 0; i < N; ++i){
        delete this->network[i];
    }

    delete[] this->network;
    delete[] this->m;

}

void PNetwork::ConnectUnits(){

    int i,j;
    std::default_random_engine generator;
    generator.seed(12345);
    //generator.seed(time(NULL));

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
                ma += ( (xi[p * j + i] == k) - a/S) * network[i]->get_state()[k];				//to calculate m
            }
            maa += ma;
        }
        this->m[i] = maa*invdenN;
    }


}
void PNetwork::Init_Units(){

    int i;

    //Generate connection matrix
    this->ConnectUnits();

    //Init unit states and r
    for(i=0; i < N; ++i){
        network[i]->init(beta,U,p,pgen->a/S,this->pgen->get_patt(),i,cm,this->network);
    }

    this->evaluate_m();



}
/*******************************************************************************
POTTS UNIT
*******************************************************************************/
PUnit::PUnit( int S, int C){
    this->state = new double[S + 1];
    this->cdata = new double[S * 2 * C * S];
    this->r = new double[S + 1];
    this->S = S;
    this->C = C;
    this->h = new double[S];
    this->theta = new double[S];
}

PUnit::~PUnit(){

    delete[] this->state;
    delete[] this->cdata;
}

double * PUnit::get_state(){
    return this->state;
}
void PUnit::init(const double beta, const double U, const int p, const double as, const int * xi, const int unit, const int * cm, PUnit ** network){

    int i,j,k,l;
    double n = -2*beta-2*exp(beta*U)-2*S+sqrt(pow(2*beta+2*exp(beta*U)+2*S,2)+8*(-beta*beta-2*beta*S+2*beta*S*exp(beta*U)));
    double d = 2*(-beta*beta-2*beta*S+2*beta*S*exp(beta*U));

    for(i = 0; i < S; ++i){
		state[i] = n / d;
	}

	state[S] = 1 - S*state[0];
	r[S] = 1 - state[S];

    //Generate Jkxl
    for(i = 0; i < S; ++i){
        h[i] = 0;
        for(j = 0; j < C; ++j){
            for(k = 0; k < S; ++k){

                cdata[i * (2*C*S) + j * (S) + k] = 0;

                //Half filled with Jkxl
                for(l = 0; l < p; ++l){
                    cdata[i * (2*C*S) + j * (S) + k] += ((xi[p * unit + l]==i)-as)*((xi[p * cm[C*unit+j] + l]==k)-as);
                }

                //Half filled with all the states that this units need to perform the update
                cdata[C*S + i * (2*C*S) + j * (S) + k] = network[cm[C*unit + j]]->state[k];

                //Compute directly initial h
                h[i] += cdata[i * (2*C*S) + j * (S) + k] * network[cm[C*unit + j]]->state[k];
            }
        }
        r[i] = h[i];
        theta[i] = state[k];
    }




}
