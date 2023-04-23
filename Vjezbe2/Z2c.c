#include <stdio.h>	// standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c"

// Podijelite Ω na kvadratnu mrežu i odredite vjerojatnost P da se cestica nađe u pojedinoj ćeliji
// mreže. Analizirajte ponašanje sustava crtajući razdiobe Nw = 250 000 šetaća za nekoliko
// odabranih k = t/∆t

// mreža je od 0 do 100 u x i u y smjeru

int main(void)
{
	int i, j;
	long idum = -1234;
	// float x[250000]; // x koordinata svakog šetača
	float x[10]; // x koordinata svakog šetača
	// float y[250000]; // y koordinata svakog šetača
	float y[10]; // y koordinata svakog šetača
	// int Nw = 250000; // broj šetača
	int Nw = 10; // broj šetača
	// int Nk = 5000; // ŠTO ĆE MI OVAJ? broj koraka?
	int Nk = 1000; // ŠTO ĆE MI OVAJ? broj koraka?
	int k = 100;	 // k = t / Dt

	FILE *file;
	file = fopen("Z2c.txt", "w");

	int n = 0; // indeks ćelije u x smjeru
	int m = 0; // indeks ćelije u y smjeru

	// for (i = 0; i < Nw; i++) // inicijaliziramo početne pozicije svih šetača
	// {
	// 	x[i] = 0;
	// 	y[i] = 0;
	// }

	int P[100][100]; // inicijaliziramo polje vjerojatnosti, mreža 100x100

	// ako je su koordinate x i y šetaća u toj i toj ćeliji, onda P[int(x)][int(y)]++; ZA SVAKI STOTI KORAK; ubače.

	// 1. svaki walker napravi svoj korak 2. svaki 100ti korak se bilježi u P arrayu lokacija++ 1.

	// inicijaliziramo polje vjerojatnosti, početne vrijednosti su 0
	for (n = 0; n < 100; n++)
	{
		for (m = 0; m < 100; m++)
		{
			P[n][m] = 0;
		}
	}

	for (i = 0; i < Nk; i++) // za svaki korak
	{
		// svakog walkera
		for (j = 0; j < Nw; j++)
		{
			// pomakni walkera x i y, dakle imat ćeš arrayeve x i y za sve šetače u tom trenutku
			x[j] = x[j] + ran1(&idum) - 0.5;
			y[j] = y[j] + 3 * ran1(&idum) - 1.5;

			// Ω = { (x, y) : | x − 50| ≤ 50, |y − 50| ≤ 50 } ⊂ R
			if (x[j] > 100)
				x[j] = 200 - x[j]; // jer 100-(x-100) = 200-x
			if (y[j] > 100)
				y[j] = 200 - y[j];
			if (x[j] < 0)
				x[j] = -1 * x[j];
			if (y[j] < 0)
				y[j] = -1 * y[j];

			if (i % k == 0)
			{

				n = (int)floor(x[j]);					 // možda n-1 stavit u P?
				m = (int)floor(y[j]);					 // možda m-1 u P?
				printf("%f %f\n", x[j], y[j]); // od
				printf("%d %d\n", n, m);
				if (n == 100)
				{
					P[n - 1][m]++;
				}
				else if (m == 100)
				{
					P[n][m - 1]++;
				}
				else
				{
					P[n][m]++;
				}
				printf("%d\n", P[n][m]);
			}
		}
	}

	for (n = 0; n < 100; n++)
	{
		for (m = 0; m < 100; m++)
		{
			// P[n][m] = 0;
			// fprintf(file, "%f %f %d\n", x[j], y[j], (int)x[j] * 10 + (int)y[j]);
			fprintf(file, "%d %d %d\n", n, m, P[n][m]);
		}
	}

	fclose(file);
	return 0;
}
