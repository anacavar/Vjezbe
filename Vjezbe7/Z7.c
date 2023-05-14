// Napravite kod koji učitava podatke iz E_dev/E.txt, dodatno ih blokira
// i procjenjuje devijacije.
// Provjerite njegovu ispravnost. Kao dokaz priložite kod i rezultate kao
// na prethodnom grafu.
// Ispišite na grafu srednju vrijednost i pripadnu standardnu devijaciju.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main(void)
{
  unsigned int ib_ns, ns = 0, ib = 0; // ib*ns
  double E[1024], Eb, sum_E, sum_E2;
  char a[128];

  FILE *data, *fout;
  data = fopen("E.txt", "r");
  fout = fopen("results.txt", "w+");

  // preskačemo prva dva reda datoteke
  fgets(a, 128, data);
  fgets(a, 128, data);

  // ib*ns i energija
  fscanf(data, "%u", &ib_ns);
  fscanf(data, "%lf", &E[ib]);
  fgets(a, 128, data);
  ns = ib_ns;

  while (ib_ns < 2000000) // isčitavamo energije
  {
    ib += 1;
    fscanf(data, "%u", &ib_ns);
    fscanf(data, "%lf", &E[ib]);
    fgets(a, 128, data);
  }

  ib++;

  int b;     // broj elemenata u bloku
  float dev; // standardna devijacija
  for (b = 1; b <= 200; b++)
  {
    int n = ib / b;
    sum_E = 0;
    sum_E2 = 0;
    for (int i = 0; i <= ib - b; i += b) // po energijama
    {
      Eb = 0;
      for (int j = 0; j < b; j++) // po jednom bloku
      {
        Eb += E[i + j] / b;
      }
      sum_E += Eb / n;
      sum_E2 += Eb * Eb / n;
    }
    dev = sqrt((sum_E2 - sum_E * sum_E) / (n - 1)) * 1000;
    fprintf(fout, "%d %lf\n", b, dev);
  }
  fclose(data);
  fclose(fout);
  return 0;
}