// Primjer MC koda za samo jedan vremenski korak radioaktivnog raspada
#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"
#define dt 0.00649212768 // min - vremenski korak
#define w 10000          // broj nezavisnih sustava
#define tmax 4000        // broj vremenskih koraka
int main()
{
  FILE *fout;
  long idum = (-2468);
  int i, j, t, Nt, k, N[w], N0 = 1000;
  float P[w];
  float suma = 0;
  float p, r, konacno = 1;
  fout = fopen("raspad.dat", "w");
  p = 0.4;
  suma = 0;
  for (i = 0; i < w; i++) // postavljanje početnog broja jezgri u sustavima
    N[i] = N0;
  t = dt;
  for (j = 0; j < w; j++) // petlja po nezavisnim sustavima (šetačima)
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
    fprintf(fout, "%d, ", N[j]); // broj raspadnutih jezgri u svakom sustavu
  }
  konacno = suma / w; // srednja vrijednost
  printf("<n> = %f", konacno);
  // Racunanje vjerojatnosti raspada n jezgara
  for (k = 1; k <= 70; k++)
  {
    P[k] = P[k] / w;
    fprintf(fout, "%d\t%f\n", k, P[k]);
  }
  fclose(fout);
  return 0;
}