#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	int my2, mx, np, Nw, jnp, i, nx, ny;
	double dmx, ro, P, o, Lx, Ly;
	double x1, y1, x2, y2, x01, x02, y01, y02, a;
	FILE *fxy;

	// ULAZNI PARAMETRI ********************************************************
	Nw = 1000; // broj konfiguracija (setaca)
	// ro=0.08;	// gustoca = broj atoma / povrsina
	ro = 0.8; // gustoca = broj atoma / povrsina
	my2 = 5;	// broj parova redaka, mjesta u y smjeru (ukupno/2)

	// ODREDJIVANJE UDALJENOSTI ************************************************
	// Atome smjestamo u vrhove jednakostranicnih trokuta duljine stranice a
	// Promatramo dio prostora priblizno oblika kvadrata:
	//   broj istih stupaca * a = broj redaka * ( visina=a*sqrt(3)/2 )
	//   broj istih stupaca     = broj parova redaka * sqrt(3)
	dmx = (double)(my2)*sqrt(3.) + 0.5; // + 0.5 da npr. 2.8 zaokruzi na 3, a ne 2
	mx = (int)dmx;											// broj mjesta u x smjeru (broj stupaca)

	np = 2 * mx * my2; // broj atoma, 2* jer su dani parovi redaka

	// povrsina = broj cestica / gustoca
	P = (double)(np) / ro;
	// odabrati a tako da Lx priblizno Ly, Lx = o*Ly
	// o = Lx / Ly = mx * a / (2*my2 * a*sqrt(3)/2)
	o = (double)mx / (my2 * sqrt(3.0));
	// Lx = sqrt(Lx*Lx) = sqrt(Lx * Ly*o) = sqrt(P*o)
	// Ly = Lx/o = sqrt(P*o)/o = sqrt(P/o)
	Lx = sqrt(P * o);
	Ly = sqrt(P / o);
	// udaljenost atoma
	a = Lx / (double)(mx);

	// RACUNANJE I POHRANJIVANJE POLOZAJA ATOMA ********************************
	fxy = fopen("xy.dat", "w");
	// Od rubova ih odmicemo za polovicu razmaka susjednih redaka (stupaca)
	// Koordinate atoma dolje lijevo (prvi u prvom i drugom retku odozdo)
	x01 = 0.25 * a; // Polozaj
	y01 = 0.25 * a * sqrt(3.);
	x02 = x01 + a * 0.5;
	y02 = y01 + a * sqrt(3.) / 2.;
	// Polozaji ostalih atoma
	for (i = 1; i <= Nw; i++)
	{
		jnp = 0;
		for (nx = 0; nx < mx; nx++)
		{
			for (ny = 0; ny < my2; ny++)
			{
				x1 = x01 + nx * a;
				y1 = y01 + ny * a * sqrt(3.);
				x2 = x02 + nx * a;
				y2 = y02 + ny * a * sqrt(3.);
				fprintf(fxy, "%f\t%f\n", x1, y1);
				fprintf(fxy, "%f\t%f\n", x2, y2);
				jnp = jnp + 2;
			}
		}
		printf("TROKUTASTA resetka\n");
		printf("%d atoma smjesteno u %d tocaka\n", np, jnp);
		printf(" - rasporedjenih u %d redaka i\n                   %d stupaca\n", my2 * 2, mx);
		printf(" - unutar podrucja:\n\t Lx=%f\n\t Ly=%f\n\t o=%f\n", Lx, Ly, o);
		printf("cime je postignuta gustoca = %f\n", (double)(jnp) / (Lx * Ly));
		printf("Prvi su susjedi udaljeni za a = %f\n", a);
	}
	fclose(fxy);
	return 0;
}
