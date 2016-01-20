#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> 
FILE *cij_input;
FILE *cij_output;

int main()
{

int N = 20;
int Cm = 4;

int i, j, x;
int A[N*Cm/2];
int B[N*Cm/2];
int **D;
int **C;

D= new int*[N];
for(i=0; i<N; i++)
D[i]=new int[N];

C= new int*[N];
for(i=0; i<N; i++)
C[i]=new int[Cm];


cij_input=fopen("C_sym_rand_dilution_N=20_Cm=4","r");
cij_output = fopen("C_sym_rand_dilution_out", "w");

for (i=0; i<N*Cm/2; i++)
{
  fscanf(cij_input,"%d	%d\n", &A[i], &B[i]);
//   printf("%d	%d\n", A[i], B[i]);
  D[A[i]][B[i]] = 1; 
  D[B[i]][A[i]] = 1; 
}    
  
for (i=0; i<N; i++)
{
  for (j=0; j<N; j++)
  {
    printf("%d ", D[i][j]);
    x=0;
    if (D[i][j] == 1) 
    {
      C[i][x] = j;
      fprintf(cij_output,"%d	", C[i][x]);
      x++;
    }
  }
printf("\n");
fprintf(cij_output, "\n");
}
}