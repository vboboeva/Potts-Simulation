#include <math.h>
#include "p_unit.h"

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
    delete[] this->r;
    delete[] this->h;
    delete[] this->theta;
}

double * PUnit::get_state(){
    return this->state;
}
void PUnit::init_states(const double beta, const double U){

    int i;
    double n = -2 * beta - 2 * exp(beta * U) - 2 * S+sqrt(pow(2 * beta + 2 * exp(beta * U)+2 * S,2)+8 * (-beta * beta - 2 * beta * S + 2 * beta *S * exp(beta * U)));
    double d = 2 * (-beta * beta - 2 * beta * S + 2 * beta * S * exp(beta * U));

    for(i = 0; i < S; ++i){
        state[i] = n / d;
    }

    state[S] = 1 - S*state[0];
    r[S] = 1 - state[S];

}
void PUnit::init_J(const int p, const double a, const int * xi, const int unit, const int * cm, PUnit ** network){

    int i,j,k,l;
    double as = a/S;

    //Generate Jkxl
    for(i = 0; i < S; ++i){
        h[i] = 0;
        for(j = 0; j < C; ++j){
            for(k = 0; k < S; ++k){

                cdata[C*S*i + S*j + k] = 0;
                //cdata[i * (2*C*S) + j * (S) + k] = 0;

                //Half filled with Jkxl
                for(l = 0; l < p; ++l){
                    cdata[C*S*i + S*j + k] += ((xi[p * unit + l]==i)-as)*((xi[p * cm[C*unit+j] + l]==k)-as);
                }
                cdata[C*S*i + S*j + k] /= a * (1 - as)* C;
                //Half filled with all the states that this units need to perform the update
                cdata[(S*C*S) + C*S*i + S*j + k] = network[cm[C*unit + j]]->state[k];
                //cdata[C*S + i * (2*C*S) + j * (S) + k] = network[cm[C*unit + j]]->state[k];

                //Compute directly initial h
                h[i] += cdata[C*S*i + S*j + k] * network[cm[C*unit + j]]->state[k];
            }
        }
        r[i] = h[i];
        theta[i] = state[i];
    }

}

void PUnit::update_rule(const int init_pattern, const double U, const double w, const double g, const double tau, const double b1, const double b2, const double b3, const double beta, const int tx, const int t){

    //tx == n0 in the old code, "time 'x' "
    int i,j,k;
    double self=0, INcost, rmax, Z;

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
                this->h[i] += this->cdata[C*S*i + S*j + k] * this->cdata[(S*C*S) + C*S*i + S*j + k];
            }
        }

        this->h[i] += w * this->state[i] - self + INcost * (init_pattern == i);

        this->theta[i] += b2 * (this->state[i]-this->theta[i]);
	    this->r[i] += b1 * (this->h[i]-this->theta[i]-this->r[i]);

        rmax = r[i] * (r[i] > rmax) - ((r[i] > rmax) - 1) * this->r[this->S];

    }

    this->r[S] += b3 * (1 - this->state[S] - this->r[S]);


    Z=0;

    for(i = 0; i < S; ++i){
        Z += exp(beta * (r[i] - rmax));
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
            cdata[(S*C*S) + C*S*i + S*index + j] = new_states[j];
        }
    }

}
