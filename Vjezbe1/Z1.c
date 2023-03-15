#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c" // ucitavamo datoteku u kojoj je definiran generator

// Osmislite i provedite test za ran1 (ili ekvivalentni generator) na temelju računanja broja susjed-susjed
// korelacija C(5). Priložite kod i graf.

int main(void)
{
  int i;
  long idum = -1234;
  float C;
  float ran[5]; // lista

  ran[0] = ran1(&idum);
  ran[1] = ran1(&idum);
  ran[2] = ran1(&idum);
  ran[3] = ran1(&idum);
  ran[4] = ran1(&idum);

  FILE *file;

  file = fopen("Z1.txt", "w");
  for (i = 0; i < 1.0E008; i++)
  {
    ran[(i + 6) % 6] = ran1(&idum);
    C += ran[(i + 1) % 6] * ran[(i + 6) % 6];
    if (i % 100000 == 0)
    {
      fprintf(file, "%d %f\n", i, fabs(-0.25 * i) / sqrt(i));
    }
  }

  fclose(file);

  return 0;
}
