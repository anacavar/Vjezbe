#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include "ran1.c"   // ucitavamo datoteku u kojoj je definiran generator

int main(void)
{
  int i, broj;
  FILE *fp = NULL; // ZAŠTO DAJEMO POINTER NA FAJL? valjda jer govorimo di da piše u memoriju...
  fp = fopen("file.txt", "w");
  for (i = 0; i < 2000; i++)
  {
    broj = rand() % 10;
    printf("%d\t %d\n", i, broj);
    fprintf(fp, "%d\t %d\n", i, broj);
  }
  return 0;
}
