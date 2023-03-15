#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>   // potrebne matematicke funkcije
#include "ran1.c"   // ucitavamo datoteku u kojoj je definiran generator

int main(void)
{
  int i;
  float x, y;
  float pi;
  int unutar, ukupno;
  float udaljenost;
  long idum1, idum2;
  idum1 = -1234;
  idum2 = -4321;
  ukupno = 10000;
  unutar = 0;
  for (i = 0; i < ukupno; i++)
  {
    x = ran1(&idum1);
    y = ran1(&idum2);
    udaljenost = sqrt((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5));
    if (udaljenost < 0.5)
    {
      unutar++;
    }
  }
  pi = 4 * ((float)unutar / (float)ukupno);
  printf("pi = %f", pi);
  return 0;
}