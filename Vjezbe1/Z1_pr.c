#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>   // potrebne matematicke funkcije

#include "ran1.c" // ucitavamo datoteku u kojoj je definiran generator

int main(void)
{
  unsigned long i;
  float r;
  float a, b, c;
  double s = 0.0; // suma
  float res;
  long idum = (-2578); // RAN: generatoru u pocetku prosljedjujemo negativni cijeli broj
  FILE *fin, *fout;
  fout = fopen("Z1_pr.txt", "w"); // datoteka za pohranu
  for (i = 0; i < 1e8; i++)
  {
    r = ran1(&idum); // RAN: pozivamo gnereator i pohranjujemo generirani broj u varijablu r

    if (i == 0)
    {
      a = r;
    }
    if (i == 1)
    {
      b = r;
    }
    if (i == 2)
    {
      c = r;
    }
    // suma
    if (i > 2)
    {
      if (i % 3 == 0)
      {
        s += a * r;
        a = r;
      }
      if (i % 3 == 1)
      {
        s += b * r;
        b = r;
      }
      if (i % 3 == 0)
      {
        s += b * r;
        b = r;
      }
    }

    if (i % 1000 == 500)
    {
      res = fabs(((s / (i - 2)) - 0.25) * sqrt(i - 2));
      fprintf(fout, "%7lu  %13.8e\n", i, res); // ispis u datoteku
    }
  }

  return 0;
}