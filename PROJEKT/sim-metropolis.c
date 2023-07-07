// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

// NpT ansambl - constant temperature and constant pressure ensemble

#define N 1000 // broj čestica
#define Nw 1   // broj šetača
// #define Nw 1000 // broj šetača
#define Nk 100 // broj koraka
// #define Nk 1000 // broj koraka
// #define Nb 100  // broj blokova

// Za simuliranje NPT ansambla,
// trebamo uzorkovati koordinate r1,r2,..,rN čestica i volumen sustava V .

// U NPT ansamblu, vjerojatnost mikrostanja je proporcionalna sa e−(E+PV )
// S obzirom da je tlak fiksan, probna konfiguracija može se generirati iz trenutne konfiguracije ili
// slučajnim pomakom čestice ili slučajnom promjenom volumena, primjerice V →V +δ(2r−1),
// gdje je r slučajno izabran iz jednolike funkcije razdiobe na jediničnom intervalu, a δ je maksi-
// malna promjena volumena. Probna konfiguracija se prihvaca ukoliko je ∆W ≤ 0 i s vjerojat-
// nosti e−∆W ukoliko je ∆W > 0. Nije nužno, a ni efikasno mijenjati volumen nakon svakog
// Monte Carlo koraka po čestici

int main(void)
{
  // Promatramo ansambl:
  // Izolirani sustav fiksnog broja čestica 𝑁, pritiska p i temperature T.
  // Glavna razlika u Monte Carlo metodi (u odnosu na NVT ansambl) je postojanje dodatnih koraka koji odgovaraju promjeni volumena

  int i, j, k;
  long idum = -1234;
  float E0 = 20.; // ukupna početna energija
  float p;        // pressure
  float T;        // temperature
  float L0 = 10;  // početna duljina brida spremnika L
  int ip;         // indeks randomly odabrane čestice
  // int ib;  // indeks bloka - trebaju li mi blokovi?

  float x[N][Nw]; // x čestica
  float y[N][Nw]; // y čestica
  float v[N][Nw]; // brzine // index čestice, indeks šetača

  float x0, y0, v0;
  float Lx, Ly;

  // plottat kako se volumen povećava/smanjuje

  float dx, dy; // promjene koordinata x i y
  float dxMax = 0.1, dyMax = 0.1;
  float dv; // promjena brzine dv
  float dvMax = 0.5;

  FILE *data, *koordinate;
  data = fopen("data.txt", "w");
  koordinate = fopen("koordinate.txt", "w");

  // inicijalizacija čestica
  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      x0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      y0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      v0 = sqrt(E0 / N);
      x[i][j] = x0;
      y[i][j] = y0;
      v[i][j] = v0;
      fprintf(koordinate, "%f\t%f\n", x0, y0);
      // dodat periodične rubne uvjete, širit L u oba smjera
    }
  }

  // ako se ne varam šetaći šeću po faznom prostor, ergo - mikrostanje
  // Za usrednjit po ansamblu
  // Ako u svakom trenutku t imaš Nw točaka faznog prostora

  // for (ib = 1; ib <= Nb; ib++) // po bloku
  // {
  // po blokovima za usrednjavanje
  // novi položaj određujemo uvijek na osnovu prethodnog pa su dobiveni
  // podaci u nekoj mjeri korelirani
  // kako bismo izbjegli utjecaj korelacija na proračun standardne devijacije
  // i sličnih veličina, podatke ćemo blokirati.
  // Blokiranje provodimo dijeleći cjelokupnu simulaciju na blokove gdje
  // svaki blok sadrži nekoliko (100, 1000, 5000, ...) koraka
  // nakon svakog bloka pohranjujemo:
  // ▶ indeks bloka ib
  // ▶ ⟨f⟩ - usrednjenu vrijednost veličine f koju smo akumulirali od početka
  // cijele simulacije
  // ▶ ⟨f⟩b - usrednjenu vrijednost veličine f koju smo akumulirali samo
  // unutar jednog bloka

  for (i = 1; i <= Nk; i++) // po koracima // ZA SVAKI KORAK PROVJERITI DA LI SE PRIHVAĆA - MOŽDA PRVO ŠETAĆI PA OVO? ILI OBRNUTO?
  {
    // printf("%d\n", i);

    for (j = 1; j <= Nw; j++) // po šetačima - mikrostanja - šetaći po 3N dimenzionalnom faznom prostoru
    {
      for (k = 1; k <= N; k++) // po česticama
      {
        // Nasumično odaberemo česticu i promijenimo joj v za nasumični broj iz [-dvMax ,dvMax]
        // ip = (int)(ran1(&idum) * N); // ran1(&idum) = random broj iz[0, 1]
        // if (ip == N)
        //   ip = N - 1;                         // ip iz {0,1,2,...,N-2,N-1} dv = (2.*ran1(&idum)-1.)*dvMax; vTmp = v + dv;
        // dv = (2. * ran1(&idum) - 1.) * dvMax; // OTKUD DOKUD?
        // vTmp = v + dv;                        // KAJ ĆE MI OVO? spremaš u temporari varijablu -> slijedi provjera :)

        // if (Ed >= dE)
        // {
        //   Ed = Ed - dE;
        //   Es = Es + dE;
        //   v = vTmp;
        // } // demon alg
        dx = (ran1(&idum) * 2 - 1) * dxMax;
        dy = (ran1(&idum) * 2 - 1) * dyMax;
        dv = (ran1(&idum) * 2 - 1) * dvMax;
        x[j][k] += dx;
        y[j][k] += dy;
        v[j][k] += dv;

        // periodični rubni uvjeti
        if (x[j][k] < 0)
          x[j][k] = -x[j][k];

        if (x[j][k] > L0)
          x[j][k] = L0 - (x[j][k] - L0);

        if (y[j][k] < 0)
          y[j][k] = -y[j][k];

        if (y[j][k] > L0)
          y[j][k] = L0 - (y[j][k] - L0);
      }
    }
  }

  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      // x0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      // y0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      // v0 = sqrt(E0 / N);
      // x[i][j] = x0;
      // y[i][j] = y0;
      // v[i][j] = v0;
      fprintf(data, "%f\t%f\n", x[i][j], y[i][j]);
    }
  }

  fclose(data);
  fclose(koordinate);

  return 0;
}