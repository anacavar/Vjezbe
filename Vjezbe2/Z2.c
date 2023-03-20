#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c"

int main(void)
{
  int i, j;
  int Nw = 5000;
  int Nk = 5000;
  long idum = -1234;
  float x[5000], y[5000];
  FILE *data;
  data = fopen("data.txt", "w");
  for (i = 0; i < Nw; i++) // inicijaliziramo početne pozicije svih walkera
  {
    // { | x − 50| ≤ 10, |y − 20| ≤ 5 } ⊂ R2
    x[i] = 20 * ran1(&idum) + 40;
    y[i] = 10 * ran1(&idum) + 15;
  }
  fprintf(data, "%d %f %f %f %f %f %f\n", 0, x[1], y[1], x[5], y[5], x[9], y[9]);

  for (i = 1; i <= Nk; i++) // za svaki korak
  {
    for (j = 1; j < Nw; j++) // za svakog walkera
    {
      // ∆rx ∈ [−0.5, 0.5] ⊂ R i ∆ry ∈ [−1.5, 1.5] ⊂ R
      x[j] = x[j] + ran1(&idum) - 0.5;
      y[j] = y[j] + 3 * ran1(&idum) - 1.5;

      // Ω = { (x, y) : | x − 50| ≤ 50, |y − 50| ≤ 50 } ⊂ R
      if (x[j] > 100)
        x[j] = 200 - x[j];
      if (y[j] > 100)
        y[j] = 200 - y[j];
      if (x[j] < 0)
        x[j] = -1 * x[j];
      if (y[j] < 0)
        y[j] = -1 * y[j];
    }
    fprintf(data, "%d %f %f %f %f %f %f\n", i, x[1], y[1], x[5], y[5], x[9], y[9]);
  }
  return 0;
}
