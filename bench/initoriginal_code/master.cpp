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


printf("genero %d\n",k);
system("icc -O3 genero_pattern.cpp -o gen.out -std=c++11");
system("./gen.out");

system("icc -O3 main_restyle.cpp -D_NO_END_CONDITION -o main.out -std=c++11");
system("./main.out");

}
