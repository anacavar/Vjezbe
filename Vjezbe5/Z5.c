#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

// double f(double x)
// {
//   return exp(-(x * x));
// }

// double p(double A, double x)
// {
//   return A * exp(-x);
// }

#define n1 5000000
#define n2 400000

int main(void)
{
  long idum = (-2578);
  int i;
  double x, suma1 = 0., suma2 = 0., Fn, sigma, g, A;

  FILE *file;
  file = fopen("rezultati.txt", "w");

  // za p(x) = 1
  for (i = 0; i < n1; i++)
  {
    g = ran1(&idum);
    x = exp(-(g * g));
    suma1 += x;
    suma2 += x * x;
  }
  Fn = suma1 / n1;
  sigma = sqrt(suma2 / n1 - Fn * Fn);
  fprintf(file, "za p(x)=1, n=%d:\nFn=%f\nsigma=%f\nsigma/sqrt(n)=%f\n\n", n1, Fn, sigma, sigma / sqrt(n1));

  // za p(x) = A*exp(-x)
  suma1 = suma2 = 0.;
  A = exp(1) / (exp(1) - 1);

  for (i = 0; i < n2; i++)
  {
    g = (-1.0) * log(1 - ran1(&idum) / A);
    x = exp(-(g * g)) / (A * exp(-g));
    suma1 += x;
    suma2 += x * x;
  }
  Fn = suma1 / n2;
  sigma = sqrt(suma2 / n2 - Fn * Fn);
  fprintf(file, "za p(x)=A*exp(-x), n=%d:\nFn=%f\nsigma=%f\nsigma/sqrt(n)=%f", n2, Fn, sigma, sigma / sqrt(n2));

  fclose(file);
  return 0;
}