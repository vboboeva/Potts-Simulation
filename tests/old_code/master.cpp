#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


int main()
{
int k;
FILE *last;
FILE *ksequenza;
FILE *Passi;

ksequenza=fopen("ksequenza.txt","w");
fclose(ksequenza);
last = fopen("last.dat","w");
fclose(ksequenza);
Passi=fopen("passi.txt","w");
fclose(Passi);

for(k=0;k<1;k++)
{
 	printf("genero %d\n",k);
	system("icpc -O2 -no-vec genero_pattern.cpp -o gen.out");
	system("./gen.out");

	system("icpc -O2 -no-vec main_restyle.cpp -o main.out -std=c++11");
	system("./main.out");
}



}
