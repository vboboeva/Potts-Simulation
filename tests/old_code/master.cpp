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
	system("icc -O0 genero_pattern.cpp -o gen.out -std=c++11");
	system("./gen.out");

	system("icc -O0 main_restyle.cpp -o main.out -std=c++11");
	system("./main.out");
}



}
