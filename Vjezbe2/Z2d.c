#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>	// standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>		// potrebne matematicke funkcije
#include "ran1.c"
int main(void)
{
	int i, j;
	double c = 0;
	float delta_x = 2, delta_y = 2;
	float x[250000];
	float y[250000];
	float P[51][51];
	int Nw = 250000;
	int Nk = 5000;
	double temp;
	float entropija;
	long idum = -1234;

	FILE *file;
	file = fopen("Z2d.txt", "w");
	int m = 0;
	int n = 0;
	float tmp;

	for (i = 0; i < Nw; i++)
	{

		x[i] = 20 * ran1(&idum) + 40;
		y[i] = 10 * ran1(&idum) + 15;
	}

	for (i = 1; i <= Nk; i++)
	{
		for (m = 0; m < 51; m++)
		{
			for (n = 0; n < 51; n++)
				P[m][n] = 0;
		}

		entropija = 0;

		for (j = 0; j < Nw; j++)
		{
			x[j] = x[j] + ran1(&idum) - 0.5;
			y[j] = y[j] + 3 * ran1(&idum) - 1.5;

			if (x[j] > 100)
				x[j] = 200 - x[j];
			if (x[j] < 0)
				x[j] = -1 * x[j];
			if (y[j] > 100)
				y[j] = 200 - y[j];
			if (y[j] < 0)
				y[j] = -1 * y[j];
			tmp = x[j] / delta_x;
			m = tmp;
			tmp = y[j] / delta_y;
			n = tmp;
			P[m][n] += 1;
		}

		if (i % 100 == 0)
		{
			for (m = 0; m < 51; m++)
			{
				for (n = 0; n < 51; n++)
				{
					P[m][n] = P[m][n] / Nw;
					float temp;
					if (P[m][n] != 0)
					{
						entropija = entropija - P[m][n] * log(P[m][n]);
					}
				}
			}
			fprintf(file, "%d %f\n", i, entropija);
		}
	}

	return 0;
}
