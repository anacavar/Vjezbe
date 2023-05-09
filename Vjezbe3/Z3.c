#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c"

// Usporedite determinističko rješenje difuzijske jednadžbe sa slučajnim hodom u 1D.
// A) Simulirajte slučajni hod za 64000 šetača koji kreću iz
// ishodišta i rade nasumične koordinatne pomake Dx e [-3, 3]cm.
// Pratite njihov hod tijekom 200 koraka. Efikasnije je simulirati
// tako da u svakom koraku pomaknete redom sve šetače.

// 1. Odredite <x2>(t) u ovisnosti o rednom broju koraka - vremena t.
// Prosjek izračunajte u svakom koraku posebno, usrednjavajući po broju šetača.

// 2. Fitanjem funkcije <x^2>=2Dt na dobivene podatke odredite koeficijent D,
// primjerice koristeći gnuplot (otipkajte apostrof '):
// >f(x)=2.0*D*x
// >fit f(x) 'x2.dat' u 1:2 via D

// iter      chisq       delta/lim  lambda   D
//    0 2.6928780614e+06   0.00e+00  2.30e+02    1.000000e+00
//    1 8.6806761484e+02  -3.10e+08  2.30e+01    1.501759e+00
//    2 8.0143370427e+02  -8.31e+03  2.30e+00    1.504268e+00
//    3 8.0143370410e+02  -2.08e-05  2.30e-01    1.504268e+00
// iter      chisq       delta/lim  lambda   D

// After 3 iterations the fit converged.
// final sum of squares of residuals : 801.434
// rel. change during last iteration : -2.07838e-10

// degrees of freedom    (FIT_NDF)                        : 199
// rms of residuals      (FIT_STDFIT) = sqrt(WSSR/ndf)    : 2.00681
// variance of residuals (reduced chisquare) = WSSR/ndf   : 4.02731

// Final set of parameters            Asymptotic Standard Error
// =======================            ==========================
// D               = 1.50427          +/- 0.0006168    (0.041%)

// 3. Odredite kako se razdioba šetača P(X) ponaša u vremenu.
// (Napraviti P(x) graf za različita vremena?)

// b) Koristeći eskplicitnu shemu, rješite difuzijsku parcijalnu diferencijalnu jednadžbu rho_t=D_rho_xx za
// rubni uvjet rho(RUB, 0) = 0 i početnu gustoću koja ne iščezava samo u ishodištu 1cm^(-1). Uzmite
// rub dovoljno daleko da u simulacijskom vremenu čestice ne difudiraju do njega, npr.
// RUB =+-100. Razlog je što nismo ograničili ni slučajni hod. Za konačno
// vrijeme možete uzeti 200s.

// c) Usporedite rješenje difuzijske jednadžbe s prethodno dobivenom
// ovisnosti P(x) za nekoliko odabranih tenutaka t=k*delta_t
//? Treba li neko od njih reskalirati ovisno o tome je li im domena diskretna
// ili kontinuirana?
//? Je li s matematičkog stajališta očekivana ekvivalentnost obaju pristupa?

#define Nx_intervala 100 // broj intervala na x osi
#define Nw 64000         // broj walkera

int main(void)
{
  int i, j, k;
  int Nk = 200;
  long idum = -1234;
  float x[Nw] = {0.}; // trenutne pozicije svih walkera...
  float dx;
  float xx; // x2
  FILE *data, *Pdata, *rhodata;
  data = fopen("data.txt", "w");       // redni broj koraka, srednja vrijednost x^2
  Pdata = fopen("Pdata.txt", "w");     // datoteka za gustoću čestica
  rhodata = fopen("rhodata.txt", "w"); // datoteka za rho

  int index_P;
  float x_max = 3 * Nk;

  int P1[Nx_intervala] = {0};
  int P2[Nx_intervala] = {0};
  int P3[Nx_intervala] = {0};

  for (i = 0; i < Nk; i++) // broj koraka, tj. po vremenu
  {
    xx = 0;

    for (j = 0; j < Nw; j++) // broj šetača, tj. po šetaču
    {
      dx = 6 * ran1(&idum) - 3; // nasumično odabiremo korak
      x[j] = x[j] + dx;         // radimo korak
      xx = xx + x[j] * x[j];

      if (i == 50) // P(x) u prvom vremenskom trenutku
      {
        index_P = (int)fabs(x[j]) * Nx_intervala / x_max; // u kojem indeksu se nalazi x walkera
        P1[index_P]++;
      }
      if (i == 100) // P(x) u drugom vremenskom trenutku
      {
        index_P = (int)fabs(x[j]) * Nx_intervala / x_max; // u kojem indeksu se nalazi x walkera
        P2[index_P]++;
      }
      if (i == 150) // P(x) u trećem vremenskom trenutku
      {
        index_P = (int)fabs(x[j]) * Nx_intervala / x_max; // u kojem indeksu se nalazi x walkera
        P3[index_P]++;
      }
    }

    xx = xx / Nw; // računamo srednju vrijednost položaja x^2 u ovom koraku

    fprintf(data, "%d %f\n", i, xx); // spremamo u datoteku redni broj koraka i srednju vrijednost x^2
  }

  for (k = 0; k < Nx_intervala; k++)
  {
    fprintf(Pdata, "%f %f %f %f\n", k * (x_max / Nx_intervala), (float)P1[k] / Nw, (float)P2[k] / Nw, (float)P3[k] / Nw);
  }

  // b)

  float dt = 1., delta_x = 4., a = 0., b;

  float rho[Nx_intervala] = {0.};
  // očuvanje sredine raspodjele
  rho[24] = 0.25;
  rho[25] = 0.5;
  rho[26] = 0.25;

  for (i = 0; i <= 200; i++)
  {
    for (int j = 0; j <= 50; j++)
    {
      if (i == 20 || i == 60 || i == 120 || i == 200)
      {
        fprintf(rhodata, "%f %f\n", j * delta_x - 100, rho[j]);
        if (j == 50)
          fprintf(rhodata, "\n\n");
      }
      if (j != 0 && j != 50)
      {
        b = rho[j];
        rho[j] = rho[j] + 1.5 * dt / delta_x / delta_x * (rho[j + 1] + a - 2 * rho[j]);
        a = b;
      }
    }
  }

  fclose(data);
  fclose(Pdata);
  fclose(rhodata);
  return 0;
}