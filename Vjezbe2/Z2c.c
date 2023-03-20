#include <stdio.h>	// standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c"

// Podijelite Ω na kvadratnu mrežu i odredite vjerojatnost P da se cestica na ˇ de u pojedinoj ¯ celiji ´
// mreže. Analizirajte ponašanje sustava crtajuci razdiobe ´ Nw = 250 000 šetaca za nekoliko ˇ
// odabranih k = t/∆t

// mreža je od 0 do 100 u x i u y smjeru

int main(void)
{
	int i, j;
	long idum = -1234;
	float x[250000];
	float y[250000];
	float P[1000]; // 100 * 100 kockica
	int Nw = 250000;
	int Nk = 5000;
	int k = 100; // k = t / Dt

	FILE *file;
	file = fopen("Z2c.txt", "w");

	int n = 0; // indeks ćelije u x smjeru
	int m = 0; // indeks ćelije u y smjeru

	for (i = 0; i < Nw; i++) // inicijaliziramo početne pozicije svih šetača
	{
		x[i] = 0;
		y[i] = 0;
	}

	// inicijaliziramo polje vjerojatnosti, početne vrijednosti su 0
	for (n = 0; n < 10; n++)
	{
		for (m = 0; m < 10; m++)
		{
			P[n * 10 + m] = 0;
		}
	}

	for (i = 1; i < Nk; i++) // za svaki korak
	{
		for (j = 0; j < Nw; j++) // za svakog walkera
		{

			// ∆rx ∈ [−0.5, 0.5] ⊂ R i ∆ry ∈ [−1.5, 1.5] ⊂ R
			x[j] = x[j] + ran1(&idum) - 0.5;
			y[j] = y[j] + 3 * ran1(&idum) - 1.5;

			// Ω = { (x, y) : | x − 50| ≤ 50, |y − 50| ≤ 50 } ⊂ R
			if (x[j] > 100)
				x[j] = 200 - x[j];
			if (y[j] > 100)
				y[j] = 200 - y[j];
			if (x[j] < 0)
				x[j] = -1 * x[j];
			if (y[j] < 0)
				y[j] = -1 * y[j];

			if (Nk % k == 0) // za svaki stoti korak, za svakog walkera
			{
				// provjeri za svaku kućicu (n i m)
				for (n = 0; n < 10; n++)
				{
					for (m = 0; m < 10; m++)
					{
						// ako su x i y unutar granica n-tog odnosno m-tog intervala P++
						if ((float)n <= x[i] && x[i] < (float)(n + 1))
						{
							if ((float)m <= y[i] && y[i] < (float)(m + 1))
							{
								P[n * 10 + m]++;
							}
						}
					}
				}
			}
		}
	}

	for (i = 0; i < Nk; i = i + 100) // za svaki korak
	{
		for (j = 0; j < Nw; j++) // za svakog walkera
		{
			fprintf(file, "%f %f %d\n", x[j], y[j], P[(int)x[j] * 10 + (int)y[j]]);
		}
	}

	fclose(file);
	return 0;
}
