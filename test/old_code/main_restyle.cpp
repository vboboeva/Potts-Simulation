///	%%%%%%%%%%%%%%%%%%		Main Restyle		%%%%%%%%%%%%%%%%%
//veloce
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <random>
#include <iomanip>
#include "const_potts.h"
#include "funzioni_restyle.cpp"
#include <cstdlib>
int       	**xi;							//pattern
double    	**s;
double    	**sold;
double    	****J;
//double	mark[N];  	/// //////////////////////////////////////////////////// per costruire una cue correlata con un pattern
double    	m[p];								//m
double    	mS[p];
int       	retr;								//pattern that I want to retrieve
double    	**h;
double		**r;					//the field
double    	q;									//activation of the network
double		**theta;						//threshold
double		n0;
//double    	den=a*(1.-as)*(double)N;		//costante al denominatore (per velocizzare)		///
double    	invdenN=1./(a*(1.-as)*(double)N);		///#####################################costante al  denominatore (per velocizzare)
double    	denCm=a*(1.-as)*(double)Cm;
double	ws=w/S;

double   	**overlap;			//mi da` l'overlap del sistema col pattern mu al tempo t
int 		**Permut;
int 		**C;
double	H, Hq, Hht, A;
double SS1[N][S+1];
double theta1[N][S];
double rr1[N][S+1];

FILE *pat;
FILE *mvari;
FILE *lulu;
FILE *last;
FILE *Qfile;
FILE *ksequenza;
FILE *Hfile;
FILE *Hqfile;
FILE *Hhtfile;
FILE *Afile;
FILE *Tfile;
FILE *R0file;
FILE *A1file;
FILE *A2file;
FILE *S1;
FILE *th1;
FILE *r1;
FILE *h1;
FILE *DTfile;
FILE *DT0file;
FILE *DSfile;
FILE *Passi;
extern void read_pattern();
extern void initializing();
extern void update_stato(int, int);
extern void update_mq(int);
extern void getmemory();
extern void deletememory();
extern void SetUpTables();
extern void print_m(double);
extern void calcolo_m();
/***************************************************    DYNAMICS   ****************************************************/

std::default_random_engine generator;
std::uniform_int_distribution<int> int_uniform_0_N(0,N-1);

int main()
{
	generator.seed(12345);

int i, n, k, f, mu, iii, ttt, x;
int  fine, intempo, numero, Mumax, Mumaxold;
double t, Mmax, lunghezza;
double Q;
time_t iniziosim, finesim, fineiniz;

ksequenza=fopen("ksequenza.txt","w");
last = fopen("last.dat","w");
//lulu = fopen("lunghezza.dat","w");
Passi=fopen("passi.txt","a");		///###########################################################################



getmemory();
read_pattern();
SetUpTables();							/// per creare vettori shaffle di unita

n0=500*N;



//for(f=0;f<p;f++)   ///	 per  diversi pattern di inizio
for(f=0;f<1;f++)   ///	 per  diversi pattern di inizio
{
lunghezza=0;
Mumaxold=p+5;
Mumax=p+5;

//f=Retr;/// !!!  f=Retr=retr
n=0;
iniziosim=time(0);
printf( "simulazione iniziata:	%ld		\n", iniziosim);
//srand48( time(NULL) );

/// poi rimetti il seme random!!!!!!!!!!!!!!!!
retr=f;
printf("prima	\n");
fprintf(ksequenza, "%d		", retr);
fflush (ksequenza);
printf("retrieval di =%d\n", retr);
initializing();						///to initializing the network
//intempo=0;

numero=-1;
printf( "inizia dinamica\n");

mvari=fopen("andamento_m.txt","w");


fineiniz=time(0);
printf( "durata inizializzazione		%ld secondi\n", fineiniz-iniziosim);

print_J("init_J.dat");

for(ttt=0;ttt<Trete;ttt++)
{
	x=(int)(NumSet*drand48());

	for(iii=0;iii<N;iii++)
	{
		//i=Permut[iii][x];
		update_stato(iii,n);																	///update di s[][] di un neu per ogni stato


		if((n%tempostampa)==0)																/// stampo gli overlap
		{
		t=(double)n/N;																			/// effective time
		calcolo_m();

// 			for(mu=0;mu<p;mu++)
// 			{
// 					fprintf(mvari, "%.2f	%.4f	%d\n", t, m[mu], mu);
// 					fflush(mvari);
// 			}

			if(n>n0+10*N)																/// stampo gli overlap
			{
				Mmax=-1.;
				Mumax=p+1;
				for(mu=0;mu<p;mu++)
				{
					if(m[mu]>Mmax)
					{
					Mmax=m[mu];
					Mumax=mu;
					}
				}

				if(Mumaxold!=Mumax && Mmax>0.5)
				{
				numero=numero+1;
				Mumaxold=Mumax;
//				printf( "t=%f\n",t);
			fprintf(ksequenza, "%d	", Mumax);
//			printf("%d	", Mumax);
			fflush (ksequenza);
				}
			}

		///per vedere se la sequenza di latching e` finita
		for(mu=0;mu<p;mu++)
		{
			fine=1;
			if(m[mu]>0.02)
			{
				fine=0;
				mu=p;
			}
		}
		if((fine!=0) && (n>n0+100*N))
		{
			lunghezza=t;
			ttt=Trete;
			iii=N;
		}
		}
	n++;
	}
if(ttt==(Trete-1))  lunghezza=t;
}

print_states("updated_states.dat");


fprintf(ksequenza, "  999999 \n");
fflush(ksequenza);

fclose(mvari);

finesim=time(0);
// printf( "simulazione finita:	%ld		\n", finesim);
printf( "durata		%ld secondi\n", finesim-iniziosim);

printf( "p=%d	retr=%d	passi %d	lunghezza = %.1f	\n",p,f, numero, lunghezza);
/*fprintf( lulu,"%d	%.1f	\n",f, lunghezza);
fflush(lulu);*/
fprintf(last,"%d	%d	%d	\n", retr, numero, Mumax);
fflush(last);

fprintf(Passi,"%d	%d	%.1f\n",f,numero,lunghezza);
fflush(Passi);
}


fclose(Passi);
deletememory();
fclose(ksequenza);
fclose(last);
//fclose(lulu);
}
/**********************************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// nella rete non fully connected se rirunno piu` volte la stessa simulazione ho sequenze diverse di latching
/// perche` uso piu` volte la funzione drand48 nel codice





///		TEMPO
///
///	 numero totale degli update= N*Trete
///
///	ttt scorre su Trete (relativo a tutta la rete)
///	iii scorre su N (relativo a singola unita)
///
/// 	n mi da il tempo di update della singola unita
///	t mi da il tempo della rete
///
