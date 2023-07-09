#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define N 1000   // broj čestica
#define Nk 10000 // broj koraka
#define Nw 50    // broj šetača
#define idm 800

int main(void)
{
  int i, j;
  int id, ip; // ip - indeks čestice
  long idum = -1234;
  float E = 20.;                  // ukupna energija
  float E_sustava[Nw + 1] = {0.}; // energija sustava
  float E_demona[Nw + 1] = {0.};  // energija demona
  float dE;                       // promjena energije sustava prilikom mijenjanja veličine v u vTmp
  float dv;                       // promjena dinamičke veličine v
  float dvMax = 2.0;              // maksimalna promjena veličine v
  float vTmp;                     // trenutna veličina v
  float Edemona_k[Nk + 1];
  float Esustava_k[Nk + 1];
  float V[N + 1][Nw + 1]; // index čestice, indeks šetača
  float DIS_Edemona[idm] = {0.};
  float dE_demona = 100;
  float sumEdemona1 = 0, sumEsustava1 = 0;
  float sumEdemona = 0, sumEsustava = 0;
  float Edemona_tot, Esustava_tot;
  float sumEdemona_k = 0, sumEsustava_k = 0;

  FILE *data, *dataEdemona, *dataEsustava, *distr;
  data = fopen("data.txt", "w");
  dataEdemona = fopen("data_Edemona.txt", "w");
  dataEsustava = fopen("data_Esustava.txt", "w");
  distr = fopen("distribution.txt", "w");

  for (int i = 1; i <= N; i++) // postavljamo početne vrijednosti
  {
    for (int j = 1; j <= Nw; j++)
      V[i][j] = sqrt(E / N);
  }

  for (i = 1; i <= Nk; i++) // po koracima
  {
    for (j = 1; j <= Nw; j++) // po šetačima
    {
      for (int k = 1; k <= N; k++) // po česticama
      {
        ip = (int)(ran1(&idum) * N + 1);
        if (ip == N + 1)
        {
          ip = N;
        }
        dv = (2. * ran1(&idum) - 1.) * dvMax;
        vTmp = V[ip][j] + dv;
        dE = vTmp * vTmp - V[ip][j] * V[ip][j];
        if (E_demona[j] >= dE)
        {
          E_demona[j] = E_demona[j] - dE;
          E_sustava[j] = E_sustava[j] + dE;
          V[ip][j] = vTmp;
        }
        id = (int)(E_demona[j] * dE_demona);
        if (id < idm)
          DIS_Edemona[id] = DIS_Edemona[id] + 1.;
        else
          printf("preskačemo ED: %d\t%lf\n", id, E_demona[j]);
      }
    }
    sumEdemona = 0;
    sumEdemona = 0;

    for (j = 1; j <= Nw; j++)
    {
      sumEdemona = sumEdemona + E_demona[j];
      sumEdemona1 = sumEdemona1 + E_demona[j];
      sumEsustava = sumEsustava + E_sustava[j];
      sumEsustava1 = sumEsustava1 + E_sustava[j];
    }

    sumEdemona = sumEdemona / Nw;
    sumEsustava = sumEsustava / Nw;
    Edemona_tot = sumEdemona1 / (Nw * i);
    Esustava_tot = sumEsustava1 / (Nw * i);
    Edemona_k[i] = sumEdemona;
    Esustava_k[i] = sumEsustava;
    fprintf(dataEdemona, "%d %f %lf\n", i, sumEdemona, Edemona_tot);
    fprintf(dataEsustava, "%d %f %lf\n", i, sumEsustava, Esustava_tot);
  }

  sumEsustava_k = 0;
  sumEdemona_k = 0;

  for (i = 1; i <= Nk; i++)
  {
    sumEsustava_k = sumEsustava_k + Esustava_k[i];
    sumEdemona_k = sumEdemona_k + Edemona_k[i];
  }

  sumEdemona_k = sumEdemona_k / Nk;
  sumEsustava_k = sumEsustava_k / Nk;
  fprintf(data, "N\tE\t<ED>\t<ES>\t<ES>/N\t<ES>/(N*<ED>)\t0.5N*<ED>\n");
  fprintf(data, "%d\t%f\t%f\t%f\t%f\t%f\t%f\n", N, E, sumEdemona_k, sumEsustava_k, sumEsustava_k / N, sumEsustava_k / (N * sumEdemona_k), 0.5 * N * sumEdemona_k);

  for (id = 0; id < idm; id++)
  {
    fprintf(distr, "%f\t%f\n", (float)id / dE_demona, DIS_Edemona[id] / 10000000);
  }

  fclose(data);
  fclose(dataEdemona);
  fclose(dataEsustava);
  fclose(distr);

  return 0;
}
