#include "lc_pnet_reduced.h"
#include "config.h"

LC_PNet_Reduced::LC_PNet_Reduced(const int & N, const int & C, const int & S, int * cm, __fpv * J) : LC_PNet(N){

    int i,j;
    this->C = C;
    this->S = S;
    this->cm = cm;

    this->J = J; //!"£"
    this->inactive_states = new __fpv[N];
    this->active_states = new __fpv[N * S];
    this->inactive_r = new __fpv[N];
    this->active_r = new __fpv[N * S];
    this->h = new __fpv[N*S];
    this->theta = new __fpv[N*S];

}
