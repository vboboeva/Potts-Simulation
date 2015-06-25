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

    delete[] this->icm;
    delete[] this->network;
    delete[] this->m;

}

void PNetwork::connect_units(){

    int i,j;
    struct uindx a;
    RandomSequence sequence(N);

    //Fill cm matrix with indices of potts units
    for(i = 0; i < N; ++i){
        //Shuffle the sequence
        sequence.shuffle(*this->pgen->generator);

        //Copy the first C numbers
        std::memcpy(cm + C*i, sequence.begin(), C * sizeof(*sequence.begin()));

        //Store in the inverse cm
        a.unit = i;
        for(j = 0; j < C; ++j){
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
                ma += ( (xi[p * j + i] == k) - a/S) * network[i]->get_state()[k];				//to calculate m
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

    //Init unit states and r
    for(i=0; i < N; ++i){
        network[i]->init(this->beta,this->U,p,pgen->a/S,this->pgen->get_patt(),i,this->cm,this->network);
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

    int i,j,t;
    double * new_states;
    RandomSequence sequence(this->N);
    std::vector<uindx>::iterator it;
    //double * patterns = this->pgen->get_patt();

    t = 0;
    //First loop = times the whole network has to be updated
    for(i = 0; i < nupdates; ++i){

        std::cout << i << std::endl;

        //Shuffle the random sequence
        sequence.shuffle(*this->pgen->generator);

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

            for (it = this->icm[j].begin(); it != this->icm[j].end(); ++it){
                this->network[it->unit]->update_cdata(new_states,it->idx);
            }

            t++;
        }
    }

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
    double n = -2 * beta - 2 * exp(beta * U) - 2 * S+sqrt(pow(2 * beta + 2 * exp(beta * U)+2 * S,2)+8 * (-beta * beta - 2 * beta * S + 2 * beta *S * exp(beta * U)));
    double d = 2 * (-beta * beta - 2 * beta * S + 2 * beta * S * exp(beta * U));

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

void PUnit::update_rule(const int init_pattern, const double U, const double w, const double g, const double tau, const double b1, const double b2, const double b3, const double beta, const int tx, const int t){

    //tx == n0 in the old code, "time 'x' "
    int i,j,k;
    double self, INcost, rmax, Z;

    rmax = this->r[this->S];

    for(i = 0; i < this->S; ++i){
        self += this->state[i];
    }
    self = (w / this->S) * self;

    INcost = (t > tx) * g * exp(-(t-tx)/tau);

    for(i = 0; i < this->S; ++i){
        h[i] = 0;

        for(j = 0; j < C; ++j){
            for(k = 0; k < S; ++k){
                this->h[i] += this->cdata[i * (2*C*S) + j * (S) + k] * this->cdata[C*S + i * (2*C*S) + j * (S) + k];
            }
        }

        this->h[i] += w * this->state[i] - self + INcost * (init_pattern == k);

        this->theta[k] += b2 * (this->state[k]-this->theta[k]);
	    this->r[k] += b1 * (this->h[k]-this->theta[k]-this->r[k]);

        rmax = r[k] * (r[k] > rmax) - ((r[k] > rmax) - 1) * this->r[this->S];

    }

    this->r[S] += b3 * (1 - this->state[S] - this->r[S]);

    Z=0;

    for(i = 0; i < S; ++i){
        Z += exp(beta * (r[k] - rmax));
    }

    Z += exp(beta * (r[S] + U - rmax));


    for(i = 0; i < S; ++i){
    	this->state[i] = exp(beta * (this->r[i] - rmax)) / Z;
    }

    this->state[S]=exp(beta * (this->r[S] - rmax + U)) / Z;

}

void PUnit::update_cdata(const double * new_states, const int index){

    int i,j;

    for(i = 0; i < S; ++i){
        for(j = 0; j < S; ++j){
            cdata[C*S + i * (2*C*S) + index * (S) + j] = new_states[j];
        }
    }

}
