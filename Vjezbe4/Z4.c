#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"
#define Nw 1000 // broj nezavisnih sustava

int main()
{
  int i, j, t, Nt, k, N[Nw] = {1000};
  long idum = (-1234);
  float P[Nw], poisson[Nw];
  float var, suma = 0, suma2 = 0;
  int faktorijel;
  float p = 0.02, halflife = 2.5, r, dt, lambda, avg = 1;
  FILE *file;
  file = fopen("raspad.txt", "w");
  lambda = log(2) / halflife;
  dt = p / lambda;

  for (j = 0; j < Nw; j++) // petlja po nezavisnim sustavima (šetačima)
  {
    Nt = 0;
    for (i = 1; i <= N[j]; i++)
    {
      r = ran1(&idum);
      if (r <= p)
        Nt++;
    }
    P[Nt] += 1; // računanje vjerojatnosti
    N[j] = Nt;
    suma += N[j];
    suma2 += N[j] * N[j];
  }

  avg = suma / Nw; // srednja vrijednost
  var = (suma2 / Nw - (suma / Nw) * (suma / Nw));

  printf("dt = %d\n<n> = %f\nvariance = %f\n", dt, avg, var);

  for (k = 1; k <= 20; k++)
  {
    P[k] = P[k] / (float)Nw;
    faktorijel = 1;
    for (int i = 1; i <= k; i++)
    {
      faktorijel = i * faktorijel;
    }

    poisson[k] = (pow(avg, k) * exp(-avg)) / faktorijel;

    fprintf(file, "%d\t%f\t%f\n", k, P[k], poisson[k]);
  }
  fclose(file);
  return 0;
}