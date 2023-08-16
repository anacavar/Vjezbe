// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define N 10 // broj čestica
#define Nw 5 // broj šetača
// #define Nw 1000 // broj šetača
#define Nk 100 // broj koraka
// #define Nk 1000 // broj koraka
// #define Nb 100  // broj blokova

float lennardJones(float x1, float x2, float y1, float y2)
{
  // float E = 1.65 * pow(10, -21); // J, za Argon
  float E = 1; // J, za Argon
  float r = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
  // float sigma = 3.4 * pow(10, -10); // m, za Argon
  float sigma = 1; // m, za Argon
  float a;
  if (r >= 1 * sigma) // jednostavno ignoriraj parove koji su preblizu tako da divergiraju - KOLKO SIGMI TU? ok :)
    a = 4 * E * (pow(sigma / r, 12) - pow(sigma / r, 6));
  else
    a = 0;
  return a;
}

// računa W = šta ono, onaj eksponent raspodjele
// ne, mislim da samo računa kao... U
float unutarnja_energija(float x[Nw + 1][N + 1], float y[Nw + 1][N + 1], int i)
{
  float potEtemp = 0;
  int a, b;
  for (a = 0; a < N; a++)
  {
    for (b = a + 1; b < N; b++)
    {
      potEtemp += lennardJones(x[i][a], x[i][b], y[i][a], y[i][b]);
    }
  }
  return potEtemp;
}

int main(void)
{
  float m = 6.69 * pow(10, -26); // kg, za Argon

  int i, j, k;
  int a, b;
  long idum = -1234;
  float E0 = 20.; // ukupna početna energija
  float pressure; // pressure
  float T = 1;    // temperature
  float k_B = 1;  // boltzmannova konstanta == kolko?
  float L0 = 10;  // početna duljina brida spremnika L
  int ip;         // indeks randomly odabrane čestice
  // int ib;  // indeks bloka TREBA LI MI TO?
  float x[Nw + 1][N + 1]; // broj šetača, broj čestica
  float y[Nw + 1][N + 1]; // broj šetača, broj čestica
  float Utemp[Nw + 1];
  float p;                    // vjerojatnost prihvaćanja promjene
  float beta = 1 / (k_B * T); // Boltzmannova konstanta
  float x0, y0, v0;
  float L[Nw + 1]; // stranica - ALI ZAŠTO Nw*??
  float V[Nw + 1]; // volumen
  float dL, dLMax = 0.5;
  float dx, dy; // promjene koordinata x i y
  float dxMax = 0.1, dyMax = 0.1;
  float V_prije, V_poslije, U_prije, U_poslije, delta_V, delta_H, delta_U;

  FILE *data, *koordinate;
  data = fopen("data.txt", "w");
  koordinate = fopen("koordinate.txt", "w");

  float postotak_prihvacanja = 0;

  // inicijalizacija čestica
  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      // dobro je da su sve čestice na početku udaljene barem za σ. (KAKO TO OSIGURATI?) možda da ih sam odjebem ak nisu?
      x0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      y0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      x[i][j] = x0;          // broj šetača, broj čestica
      y[i][j] = y0;          // broj šetača, broj čestica
      // JER GLEDAMO SAMO UNUTARNJU ENERGIJU!!! SRC: WIKIPEDIA: https://en.wikipedia.org/wiki/Isothermal%E2%80%93isobaric_ensemble
      fprintf(koordinate, "%f\t%f\n", x0, y0);
    }
    // računamo unutarnju energiju početne konfiguracije
    Utemp[i] = unutarnja_energija(x, y, i); // indeks šetača
    printf("%f\n", Utemp[i]);
    L[i] = L0;
    V[i] = L0 * L0;
  }

  for (i = 1; i <= Nw; i++) // po šetačima - mikrostanja - šetaći po 3N dimenzionalnom faznom prostoru
  {
    // printf("%d\n", i);
    for (j = 1; j <= Nk; j++) // po koracima
    {
      V_prije = V[i];
      U_prije = unutarnja_energija(x, y, i);

      for (k = 1; k <= N; k++) // po česticama
      {
        dx = (ran1(&idum) * 2 - 1) * dxMax;
        dy = (ran1(&idum) * 2 - 1) * dyMax;
        x[i][k] += dx; // broj šetača, broj čestica
        y[i][k] += dy; // broj šetača, broj čestica

        // rubni uvjeti
        if (x[i][k] < 0)
          x[i][k] = -x[i][k];
        if (x[i][k] > L0)
          x[i][k] = L0 - (x[i][k] - L0);
        if (y[i][k] < 0)
          y[i][k] = -y[i][k];
        if (y[i][k] > L0)
          y[i][k] = L0 - (y[i][k] - L0);
      }

      if (j % 5 == 0) // SVAKI N-ti korak promijeni i volumen... (npr N=5)
      {
        dL = (ran1(&idum) * 2 - 1) * dLMax;
        L[i] = L[i] + dL;
        V[i] = L[i] * L[i];
        for (k = 1; k <= N; k++) // po česticama
        {
          x[i][k] = L[i] / (L[i] - dL) * x[i][k]; // skaliranje svih x koordinata faktorom L'/L
          y[i][k] = L[i] / (L[i] - dL) * y[i][k]; // skaliranje svih y koordinata faktorom L'/L
        }
      }

      // račun vjerojatnosti prihvaćanja p
      V_poslije = V[i]; // provjeri
      U_poslije = unutarnja_energija(x, y, i);
      delta_V = V_poslije - V_prije;
      delta_U = U_poslije - U_prije;
      delta_H = delta_U + pressure * delta_V - k_B * T * N * log(V_poslije / V_prije);
      p = exp(-beta * delta_H); // vjerojatnost prihvaćanja promjene (beta = 1/(k_B * T))
      // KOJE VRIJEDNOSTI POSTAVITI DA FIZIKALNO IMA SMISLA?
      printf("delta_H = %f\n", delta_H);
      printf("p = %f\n", p);

      // PRIHVAĆA LI SE KORAK ILI ODBACUJE? (na temelju p)
    }
  }

  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      fprintf(data, "%f\t%f\n", x[i][j], y[i][j]);
    }
  }

  fclose(data);
  fclose(koordinate);

  return 0;
}
