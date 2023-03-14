#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include "ran1.c"   // ucitavamo datoteku u kojoj je definiran generator

int main(void)
{
  int i;
  float broj;
  long idum = (-2578); // RAN: generatoru u pocetku prosljedjujemo negativni cijeli broj
  for (i = 0; i < 1000; i++)
  {
    broj = ran1(&idum) * 24.4 - 1; // RAN: pozivamo generator i pohranjujemo generirani broj u varijablu r
    printf("%f\n", broj);
  }
  return 0;
}
