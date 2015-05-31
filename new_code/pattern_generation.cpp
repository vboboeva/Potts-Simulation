#include "main.h"


PatternGen::PatternGen(int N_fact, int Num_fact, int p, int N){
    this->N_fact = N_fact;
    this->Num_fact = Num_fact;
    this->p = p;
    this->N = N;
}

void PatternGen::generate(){

    int N_p,i,ii,k,m,s1,u1,u2,unit;
    float y, h_max, eigen_fact, sum_e, piccolo, a_pa,  a_patt, dh, h000,expon,fluct,bb;
    float hh[Num_u][Num_s],hhh[Num_s],ss[Num_s];

    //Set factors
    for(k=0; k<Num_fact; k++){
        for(i=0; i<N_fact; i++){

            Factors[i][k] = (int)((double)Num_u*drand48());

        }
    }


    //Set patterns
    piccolo = log(eps);
    a_patt = 0.0;

    for(m=0; m<Num_p; m++){

        for(i=0; i<Num_u; i++){
            for(s1=0; s1<Num_s; s1++){

                hh[i][s1] = 0.0;

            }
        }


        k = 0;
        expon = 0.0;

        while(expon > piccolo){
            expon = -fact_eigen_slope*k;
            if((k+2) > Num_fact)expon = 2.*piccolo;
            y = (float)drand48();
            if(y<=a_pf)
            {
                eigen_fact = exp(expon)*y/a_pf;
                s1 = (int)((double)Num_s*drand48());
                for(ii=0; ii<N_fact; ii++)
                {
                    hh[Factors[ii][k]][s1] += eigen_fact+eps*drand48();
                }
            }
            k++;
        }



        a_pa = 0.0;
        h000 = Num_fact;
        k = 0;

        if(fact_eigen_slope > (1./(float)Num_fact)){
            h000 = 1./fact_eigen_slope;
        }

        h000 *= 0.5*a_pf*a_fact/(float)Num_s;
        bb = beta;

        while(((a_pa-a_mod)*(a_pa-a_mod))>=eps){

            N_p = 0;
            fluct = sqrt(eps)*(float)k;

            for(unit=0;unit<Num_u;unit++){

                h_max = 0.0;

                for(s1=0; s1<Num_s; s1++){
                    hhh[s1] = hh[unit][s1]+fluct*drand48();
                    if(hhh[s1]>h_max)h_max = hhh[s1];
                }

                sum_e = 0.0;

                for(s1=0;s1<Num_s;s1++){
                    dh = hhh[s1] - h_max;
                    if(dh>piccolo)sum_e += exp(bb*dh);
                }

                dh = h000 -h_max;

                if(dh>piccolo){
                    sum_e += exp(bb*dh);
                }

                Patt[m][unit] = Num_s;

                for(s1=0;s1<Num_s;s1++){

                    ss[s1] = exp(bb*(hhh[s1]-h_max))/sum_e;

                    if(ss[s1]>=0.5){
                        N_p++;
                        Patt[m][unit] = s1;
                    }
                }
            }

        a_pa = (float)N_p/(float)Num_u;
        h000 += 0.1*(a_pa-a_mod);

        k++;

        }

       a_patt += a_pa/(float)Num_p;

    }

    std::cout << "Average pattern sparsity" << a_patt << std::endl;

}

void PatternGen::eval_stats(){

    int mu, nu, i;
    double C[p][p], coppie = 0, media = 0, varianza = 0, mC, vC;

    //Evaluating correlation
    for(mu=0;mu<p;mu++){
        for(nu=0;nu<p;nu++){

            C[mu][nu]=0.0;

            for(i=0;i<N;i++) C[mu][nu]+= (Patt[mu][i]==Patt[nu][i]);

            C[mu][nu]=C[mu][nu]/N;

        }

    }

    //Mean and standard deviation
    for(mu=0;mu<(p-1);mu++){
        for(nu=(mu+1);nu<p;nu++){

            media+= C[mu][nu];

            coppie++;

        }

    }

    mC=media/coppie;

    for(mu=0;mu<(p-1);mu++){
        for(nu=(mu+1);nu<p;nu++){

        varianza+= C[mu][nu]* C[mu][nu]-mC*mC;

        }
    }

    vC=varianza/coppie;

    //Display output
    std::cout << "<C>: Mean="
              << std::fixed
              << std::setw( 11 )
              << std::setprecision( 4 )
              << mC
              << "  Standard Dev="
              << sqrt(vC)
              << std::endl;

}
