#include <iostream>
#include <time.h>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
///	%%%%%%%%%%%%%%%%%%		CONNECTIVITY		%%%%%%%%%%%%%%%%%
extern int    	 	**xi;
extern double   	**s;
extern double    	**sold;
extern double    	****J;		/// 33333333333333333333333
//extern double	mark[N]; 				/// per costruire una cue correlata con un pattern
extern double    	m[p];
extern double    	mS[p];
extern int       	retr;
extern double    	**h;
extern double		**r;
extern double    	q;
extern double		**theta;
extern double		n0;
extern double    	invdenN;
extern double    	denCm;
extern double   	**overlap;
extern int 		**Permut;
extern int 		**C;		/// 33333333333333333333333
extern double		H, Hq, Hht, A;
extern double	ws;
extern double SS1[N][S+1];
extern double theta1[N][S];
extern double rr1[N][S+1];

extern FILE *pat;
extern FILE *mvari;
extern FILE *Hfile;
extern FILE *Hqfile;
extern FILE *Hhtfile;
extern FILE *Afile;
extern FILE *Tfile;
extern FILE *DTfile;
extern FILE *DT0file;
extern FILE *DSfile;
extern FILE *R0file;
extern FILE *A1file;
extern FILE *A2file;
extern FILE *S1;
extern FILE *th1;
extern FILE *r1;
extern FILE *h1;
extern FILE *Qfile;

void print_m(double tempo)
{
int mu;

for(mu=0;mu<p;mu++)
{
	fprintf(mvari, "%.2f	%.4f	%d\n", tempo, m[mu], mu);
}

}

void read_pattern()
{
int i, mu;
pat=fopen("pattern.dat","r");
//	pat=fopen("spatt.txt","r");
for(mu=0;mu<p;mu++)
{
	for(i=0;i<N;i++)
	{
		fscanf(pat, "%d", &xi[i][mu]);
	}
}
fclose(pat);
}

void print_states(std::string filename){

	int i,k;
	std::ofstream ofile;
	ofile.open(filename);
	ofile.precision(15);
    ofile << std::scientific;
	for(i=0;i<N;i++)
	{
		for(k=0;k<S;k++)
		{
		ofile << s[i][k] << " ";
		}
	ofile << s[i][S] << " "<< std::endl;
	}
	ofile.close();
}

void print_connectivity(){
	int i;
	int i_c;
	std::ofstream ofile;
	ofile.open("init_connections2.dat");
	for(i=0; i<N; i++)
{
	for(i_c=0; i_c<Cm;i_c++)
	{
		ofile << C[i][i_c] << " ";
	}
	ofile << std::endl;
}

ofile.close();

}
void print_J(std::string filename){

	int i,j,k,l;
	std::ofstream ofile;
	ofile.open(filename);
	ofile.precision(15);
    ofile << std::scientific;
	for(i = 0; i < N; ++i){
        for(j = 0; j < S; ++j){
            for(k = 0; k < Cm; ++k){
                for(l = 0; l < S; ++l){
                    ofile << J[i][k][j][l] << " ";
                }
            }
        }
        ofile << std::endl;
    }
	ofile.close();
}

void read_connectivity()
{
	int i, c;
	FILE * con;
	con=fopen("init_connections.dat","r");
	//	pat=fopen("spatt.txt","r");
	for(i=0;i<N;i++)
	{
		for(c=0;c<Cm;++c)
		{
			fscanf(con, "%d", &C[i][c]);
		}
	}
	fclose(con);
}

void initializing()
{
int i, j, l, k, mu;
double qs, ma, maa;
int i_c, x, new_one;

/// inizializzo da file


for(i=0;i<N;i++)
{
	for(k=0;k<S;k++)
	{
	s[i][k]=(-2*beta-2*exp(beta*U)-2*S+sqrt(pow(2*beta+2*exp(beta*U)+2*S,2)+8*(-beta*beta-2*beta*S+2*beta*S*exp(beta*U))))/(2*(-beta*beta-2*beta*S+2*beta*S*exp(beta*U)));  //soluzione per lo stato sazionario sviluppata attorno a s[i][k]=0
	}
s[i][S]=1.-S*s[i][0];
r[i][S]=1.-s[i][S];
}

print_states("init_states.dat");


///		Cij		///33333333333333333333333333333333333333333
read_connectivity();
printf("dopo	\n");

print_connectivity();
/// stampo la matrice delle connessioni  Cij
// FILE *cij;
// cij=fopen("Cij.txt","w");
//
// for(i=0; i<N; i++)
// {
// 	for(x=0; x<Cm; x++)
// 	{
// 		fprintf(cij,"%d	",C[i][x]);
// 	}
// fprintf(cij,"	\n");
// }
// fclose(cij);


///		Jixkl		///33333333333333333333333333333333333333333


for(i=0; i<N; i++)
{
	for(x=0; x<Cm; x++)
	{
		for(k=0; k<S; k++)
		{
			for(l=0; l<S; l++)
			{
			J[i][x][k][l]=0;

			for(mu=0; mu<p; mu++)
					J[i][x][k][l]+=((double)(xi[i][mu]==k)-as)*((double)(xi[C[i][x]][mu]==l)-as);
			J[i][x][k][l]=J[i][x][k][l]/denCm;
//			J[i][x][k][l]=(J[i][x][k][l]*(double)((k==0)*(l==0)))/denCm;
			}
		}
	}
}
printf("dopo3	\n");


///		M		///33333333333333333333333333333333333333333

for(mu=0;mu<p;mu++)
{
	maa=0.;
	for(i=0;i<N;i++)
	{
		ma=0.;
		for(k=0;k<S;k++)
		{
			ma+=((double)(xi[i][mu]==k)-as)*s[i][k];				//to calculate m
		}
		maa+=ma;
	}
m[mu]=maa*invdenN;									//value of m[mu] for each mu
}

/*
///		H , R , T 		///33333333333333333333333333333333333333333
for(i=0;i<N;i++)								//to calculate h, r, theta
{
	r[i][S]=0.;

	for(k=0;k<S;k++)
	{
	h[i][k]=0.;
	 for(x=0;x<Cm;x++)
	{
		for(l=0;l<S;l++)
		{
		 h[i][k]+=	J[i][x][k][l]*s[C[i][x]][l];	///	333333333333333333333333333333333333333
		}
	}
	r[i][k]=h[i][k];
	theta[i][k]=s[i][k];

	}
}*/

///		H , R , T 		///33333333333333333333333333333333333333333
for(i=0;i<N;i++)								//to calculate h, r, theta
{
	for(k=0;k<S;k++)
	{
	h[i][k]=0.;
	 for(x=0;x<Cm;x++)
	{
		for(l=0;l<S;l++)
		{
		 h[i][k]+=	J[i][x][k][l]*s[C[i][x]][l];	///	333333333333333333333333333333333333333
		}
	}
	r[i][k]=h[i][k];			//valgono nello stato stazionario
	theta[i][k]=s[i][k];		//valgono nello stato stazionario
	}
}

}

void update_stato(int i, int n)
{
int   mu, k, x, v, l;
double Z, rmax, t;
double II[p], self;
double INcost;
double mII;
int MKcost;

rmax=r[i][S];


///	update per tutti gli stati diversi da S	///////
self=0.;															// calcolo la auto eccitazione
for(l=0;l<S;l++)
{
	self+=s[i][l];
}
self=ws*self;

INcost	=	(double)(n>n0)*g*exp(-((n-n0)/((double)tau)));			/// campo iniziale &&&&&&&&&&&&@@@@@@@@@@@@@@@@@@

for(k=0;k<S;k++)
{
	///	di  h
	h[i][k]=0.;
	 for(x=0;x<Cm;x++)
	{
		for(l=0;l<S;l++)
		{
		 h[i][k]+=	J[i][x][k][l]*s[C[i][x]][l];
		}
	}
	h[i][k]+=w*s[i][k]-self+ INcost*(xi[i][retr]==k);										//tolgo l`auto eccitazione
	/// di sold, thteta, r
	sold[i][k]=s[i][k];

	theta[i][k]+=b2*(s[i][k]-theta[i][k]);								//update theta
	r[i][k]+=b1*(h[i][k]-theta[i][k]-r[i][k]);								//update r

	if(r[i][k]>rmax)												//(per evitare l'overflow calcolando s)
	{
		rmax=r[i][k];
	}

}

/// //////////	update rS e sold per S	///
sold[i][S]=s[i][S];
r[i][S]+=b3*(1.-s[i][S]-r[i][S]);

/// //////////    UPDATE stato PER T!=0    ///////////////
Z=0.;
for(k=0;k<S;k++)
{
	Z+=exp(beta*(r[i][k]-rmax));
}
Z+=exp(beta*(r[i][S]+U-rmax));							//modificato con nuova concezione di U

double invZ;

//invZ=1./Z;

for(k=0;k<S;k++)
{
	s[i][k]=exp(beta*(r[i][k]-rmax))/Z;
	/*
	if(i == 0){
		std::cout.precision(30);
					std::cout << std::scientific;
					std::cout << r[i][k] << std::endl;
				}					//update of s[]
				*/
}
s[i][S]=exp(beta*(r[i][S]-rmax+U))/Z;


/*
/// //////////    UPDATE H
t=(double)n/N;
//if((n%tempostampa)==0 && n>=10000*N)
if((n%tempostampa)==0)
{

H=0.;
Hq=0.;
Hht=0.;
	for(v=0;v<N;v++)
	{
		for(k=0;k<S;k++)
		{
		Hq+=-h[v][k]*s[v][k]/2.;   //manca k=0!!!!!
		Hht+=-h[v][k]*s[v][k]/2.+theta[v][k]*s[v][k];
		H+=-h[v][k]*s[v][k]/2.+(U+theta[v][k]+r[v][S])*s[v][k];  //il contributo di w e` gia` dentro h
		}
//	H+=-(theta[v][S]+U);
	}
fprintf(Hqfile, "%.2f	%.4f\n", t, Hq);
fprintf(Hfile, "%.2f	%.4f\n", t, H);
fprintf(Hhtfile, "%.2f	%.4f\n", t, Hht);
fflush(Hqfile);
fflush(Hfile);
fflush(Hhtfile);

double A1, A2;
A=0.;
A1=0.;
A2=0.;

for(v=0;v<N;v++)
{
	for(k=0;k<S;k++)
	{
	A1+=0.5*(theta[v][k]*theta[v][k]-2*theta[v][k]*s[v][k]);
	}
A2+=0.5*(r[v][S]*r[v][S]-2*r[v][S]*(1-s[v][S]));
}
A=A1+A2;


fprintf(Afile, "%.2f	%.2f\n", t, A);
fprintf(A1file, "%.2f	%.2f\n", t, A1);
fprintf(A2file, "%.2f	%.2f\n", t, A2);
fflush(Afile);
fflush(A1file);
fflush(A2file);

double TT, R0;
TT=0.;
R0=0.;
for(v=0;v<N;v++)
{
	for(k=0;k<S;k++)
	{
	TT+=theta[v][k]/(double)N;
	}
R0+=r[v][S]/(double)N;
}
fprintf(Tfile, "%.2f	%.2f\n", t, TT);
fprintf(R0file, "%.2f	%.2f\n", t, R0);
fflush(Tfile);
fflush(R0file);


/// STAMPO DERIVATE

double DT, DT0, DS;

DT=0.;
DT0=0.;
DS=0.;

for(v=0;v<N;v++)
{
	for(k=0;k<S;k++)
	{
	DT+=b2*fabs(s[v][k]-theta[v][k]);
	DS+=fabs(s[v][k]-sold[v][k]);
	}
DT0+=b3*fabs(1.-s[v][S]-r[v][S]);
DS+=fabs(s[v][S]-sold[v][S]);
}


// fprintf(DTfile, "%.2f	%.6f\n", t, b2*(s[2][1]-theta[2][1]));
// fprintf(DSfile, "%.2f	%.6f\n", t, s[2][1]-sold[2][1]);
// fprintf(DT0file, "%.2f	%.6f\n", t, s[2][1]);
// fprintf(Tfile, "%.2f	%.6f\n", t, theta[2][1]);
 fprintf(DTfile, "%.2f	%.6f\n", t, DT);
// fprintf(DT0file, "%.2f	%.6f\n", t, DT0);
 fprintf(DSfile, "%.2f	%.6f\n", t, DS);

fflush(DTfile);
fflush(DT0file);
fflush(DSfile);
}*/
}



//printf("%.6f\n", s[i][k]-sold[i][k]);
//printf("	%.6f\n", DS);
//DS+=b3*fabs(h[v][k]-theta[v][k]-r[v][k]);







void calcolo_m()
{
int  k,i, l, mu;
double ma, maa;
///		M		///33333333333333333333333333333333333333333
for(mu=0;mu<p;mu++)
{
	maa=0.;
	for(i=0;i<N;i++)
	{
		ma=0.;
		for(k=0;k<S;k++)
		{
			ma+=((double)(xi[i][mu]==k)-as)*s[i][k];				//to calculate m
		}
		maa+=ma;
	}
m[mu]=maa*invdenN;									//value of m[mu] for each mu

}


}

void getmemory()
{
int i, x, z;

Permut= new int*[N];
for(i=0; i<N; i++)
	Permut[i]=new int[NumSet];

xi= new int*[N];
for(i=0; i<N; i++)
	xi[i]=new int[p];

s= new double*[N];
for(i=0; i<N; i++)
	s[i]=new double[S+1];

sold= new double*[N];
for(i=0; i<N; i++)
	sold[i]=new double[S+1];

h= new double*[N];
for(i=0; i<N; i++)
	h[i]=new double[S];

r= new double*[N];
for(i=0; i<N; i++)
	r[i]=new double[S+1];

theta= new double*[N];
for(i=0; i<N; i++)
	theta[i]=new double[S];
/*
overlap= new double*[tstampato];
for(i=0; i<tstampato; i++)
	overlap[i]=new double[p];
*/
J= new double***[N];
for(i=0; i<N; i++)
{
	J[i]=new double**[N];
	 for(x=0; x<Cm; x++)
	{
		J[i][x]=new double*[S];
		 for(z=0; z<S; z++)
			J[i][x][z]=new double[S];
	}
}



C= new int*[N];
for(i=0; i<N; i++)
	C[i]=new int[Cm];

}

void deletememory()
{
int i;

for(i=0; i<N; i++)
	delete (Permut[i]);
delete(Permut);

for(i=0; i<N; i++)
	delete (xi[i]);
delete(xi);


for(i=0; i<N; i++)
	delete (s[i]);
delete(s);

for(i=0; i<N; i++)
	delete (sold[i]);
delete(sold);

for(i=0; i<N; i++)
	delete (h[i]);
delete(h);

for(i=0; i<N; i++)
	delete (r[i]);
delete(r);

for(i=0; i<N; i++)
	delete (theta[i]);
delete(theta);
/*
for(i=0; i<tstampato; i++)
	delete (overlap[i]);
delete(overlap);
*/
}

void SetUpTables()
{
int item, jtem, info;
int fatto, kk;

//srand48(time(0));
srand48(6937);
for(kk=0; kk<NumSet; kk++)
{
	item = 0;

	while(item<N)
	{


	info =(int)((double)N*drand48());

	fatto=0;
	while(fatto==0)
	{

		fatto=1;

		for(jtem=0; jtem<item; jtem++)
		{
			if(  Permut[jtem][kk] == info  )
			{
				info=(info +1)-(int)((info+1)/N)*N;
				jtem=item;
				fatto=0;
			}
		}


	}

	Permut[item][kk]= info;
	item++;
	}

}

}
