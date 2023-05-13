/* ============================================================================================
	 _______________________ ______  __   ______________________  ___
	 __  ___/___  __/__  __ \___  / / /   __  ___/____  _/___   |/  /
	 _____ \ __  /   _  / / /__  /_/ /    _____ \  __  /  __  /|_/ /
	 ____/ / _  /    / /_/ / _  __  /     ____/ / __/ /   _  /  / /
	 /____/  /_/     \____/  /_/ /_/      /____/  /___/   /_/  /_/

			Prirodoslovno-matematicki fakultet u Splitu
			Stohasticke simulacije u klasicnoj i kvantnoj fizici
			Metropolis algoritam :: <r> za Vodikov atom u stanju |2,1,0>
			2011/2012, 2015/2016, 2017/18, 2018/2019, 2019/20, 2020/21, 2022/23

Koristene oznake (legend):

			Nb    = broj blokova (number of blocks)
			ib    = indeks bloka, trenutni blok (index of block)
			Nk    = broj koraka (number of steps)
			ik    = indeks koraka, trenutni korak (index of step)
			Nw    = broj setaca (number of walkers)
			iw    = indeks setaca (index of walker)
			NbSkip= broj preskocenih blokova (number od skipped blocks)

			f     = vrijednost funkcije f (function value)
			Swf   = zbroj vrijednosti od f po svim setacima (sum of average f by walkers)
			Skf   = zbroj srednjih vrijednosti po koracima (sum of average f by steps)
			Sbf   = zbroj srednjih vrijednosti po blokovima (sum of average f by blocks)

			acc   = brojac prihvacenih koraka (counter of accepted steps)
			accP  = udio prihvacenih koraka (rate of accepted steps)
			Nacc  = broj koraka za provjeru prihvacanja (total steps)

			x     = koordinate polozaja setaca (position coordinates of walker)
			dx    = promjena koordinate (random change of coordinate)
			dX    = maksimalna duljina koraka (maximum change of coordinate)
			xp    = koordinate probnog polozaja setaca (trial position)
			rp    = r probnog polozaja (r of xp)
			P     = vjerojatnot nalazenja na polozaju x (probability density at x)
			Pp    = vjerojatnost nalazenja u probnom polozaju (probability density at xp)
			T     = vjerojatnost prijelaza x -> xp (probability of transition x -> xp)
			Psi   = valna funkcija (wavefunction)

============================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define NbSkip 50 // broj koliko prvih blokova preskačemo
#define Nb 200		// broj blokova koje uzimamo u obzir
#define Nk 100		// broj koraka
#define Nw 100		// broj šetača
#define Nacc 100	// broj koraka za provjeru prihvaćanja

int main()
{
	long idum = (-2123);
	int ik = 0, ib = 1, iw, k;
	double acc, accP, ave_f, sig_f;
	double x[4][Nw + 1], xp[4], dX[4], f[Nw + 1], P[Nw + 1], Psi(double, double);
	double dx, rp, rp2, Swf, Skf, Sbf, Sbf2, Pp, T, tmp;
	FILE *ff, *frNw;
	FILE *frNw0;
	// datoteke za srednje vrijednosti i polozaje setaca (files for averages and positions)
	ff = fopen("r.dat", "w");
	frNw = fopen("rNw.dat", "w");
	frNw0 = fopen("rNw0.dat", "w");

	// inicijalizacija (initialization)
	dX[1] = dX[2] = dX[3] = 1.5;
	for (iw = 1; iw <= Nw; iw++)
	{
		rp2 = 0.;
		for (k = 1; k <= 3; k++)
		{
			x[k][iw] = 14. * (ran1(&idum) - 0.5);
			rp2 = rp2 + x[k][iw] * x[k][iw];
		}
		// fprintf(frNw, "%7d  %13.+8e  %13.8e  %13.8e\n", (ib - 1) * Nk + ik, x[1][iw], x[2][iw], x[3][iw]);
		fprintf(frNw0, "%7d  %13.8e  %13.8e  %13.8e\n", (ib - 1) * Nk + ik, x[1][iw], x[2][iw], x[3][iw]);
		rp = sqrt(rp2);
		tmp = Psi(rp, x[3][iw]);
		P[iw] = tmp * tmp;
		f[iw] = rp;
	}
	// fprintf(frNw, "\n\n");
	fprintf(frNw0, "\n\n");
	fclose(frNw0);

	acc = 0.;
	Sbf = 0.;
	Sbf2 = 0.;
	for (ib = 1 - NbSkip; ib <= Nb; ib++)
	{ // blokovi (blocks)
		Skf = 0.;
		if (ib == 1)
			acc = 0.;
		for (ik = 1; ik <= Nk; ik++)
		{ // koraci (steps)
			Swf = 0;
			for (iw = 1; iw <= Nw; iw++)
			{ // setaci (walkers)
				rp2 = 0.;
				for (k = 1; k <= 3; k++)
				{ // koordinate (coordinates)
					dx = 2. * (ran1(&idum) - 0.5) * dX[k];
					xp[k] = x[k][iw] + dx;
					rp2 = rp2 + xp[k] * xp[k];
				}
				rp = sqrt(rp2);
				tmp = Psi(rp, xp[3]);
				Pp = tmp * tmp;
				T = Pp / P[iw];
				if (T >= 1)
				{
					for (k = 1; k <= 3; k++)
						x[k][iw] = xp[k];
					acc += 1.;
					P[iw] = Pp;
					f[iw] = rp;
				}
				else if (ran1(&idum) <= T)
				{
					for (k = 1; k <= 3; k++)
						x[k][iw] = xp[k];
					acc += 1.;
					P[iw] = Pp;
					f[iw] = rp;
				}
				Swf = Swf + f[iw];
				//////////////////////////////////////////////////////
				if (ib % 100 == 0 && ik == Nk)
				{
					for (iw = 1; iw <= Nw; iw++)
					{
						fprintf(frNw, "%7d  %13.8e  %13.8e  %13.8e\n", (ib - 1) * Nk + ik, x[1][iw], x[2][iw], x[3][iw]);
					}
				}

			} // setaci (walkers)
			// duljinu koraka podesavamo za prihvacanje = 50% (adjusting step length to get acceptance = 50%)
			if (((ib - 1 + NbSkip) * Nk + ik) % Nacc == 0 && ib < 1)
			{ // tijekom stabilizacije (during stabilization)
				accP = acc / (Nacc * Nw);
				// 1) PRILAGODITE MAX KOORDINATNE POMAKE (ADJUST MAX COORDINATE CHANGES)
				if (accP > 0.5)
				{
					dX[1] = dX[1] * 1.05;
					dX[2] = dX[2] * 1.05;
					dX[3] = dX[3] * 1.05;
				}
				if (accP < 0.5)
				{
					dX[1] = dX[1] * 0.95;
					dX[2] = dX[2] * 0.95;
					dX[3] = dX[3] * 0.95;
				}

				if (ib % 10)
					printf("ib = %d, accP = %3.1lf \%\n", ib, accP * 100.);
				acc = 0.0;
			}
			if (ib > 0)
			{
				Skf += Swf / Nw; // nakon stabilizacije (after stabilization)
				/////////////////////////////////////////////////////////////////////////
				accP = acc / (ib * Nw * Nk);
				if (accP > 0.5)
				{
					dX[1] = dX[1] * 1.05;
					dX[2] = dX[2] * 1.05;
					dX[3] = dX[3] * 1.05;
				}
				if (accP < 0.5)
				{
					dX[1] = dX[1] * 0.95;
					dX[2] = dX[2] * 0.95;
					dX[3] = dX[3] * 0.95;
				}
			}
			//////////////////////////////
			if (ib > 0)
			{
				Skf += Swf / Nw;
				if (ib % 100 == 0 && ik == Nk)
					fprintf(frNw, "\n\n");
			}
		} // koraci(steps)
		if (ib > 0)
		{
			Sbf += Skf / Nk;
			Sbf2 += Skf * Skf / (Nk * Nk);
			// accP = acc / (ib * Nw * Nk);
			// 2) PRILAGODITE MAX KOORDINATNE POMAKE (ADJUST MAX COORDINATE CHANGES)
			// u petlji gore

			if (ib % (Nb / 10) == 0)
				printf("ib = %d, accP = %3.1lf \%\n", ib, accP * 100.);
			// 3) POHRANITE SVE POLOZAJE U frNw
			// 3) (STORE ALL POSITIONS IN frNw)
			// u petlji gore

			// fprintf(frNw, "\n\n");
			fprintf(ff, "%7d  %13.8e  %13.8e\n", ib, Skf / Nk, Sbf / ib);
		}
	} // blokovi (blocks)

	// 4) IZRACUNAJTE PROSJEK I DEVIJACIJU: ave_f, sig_f (CALCULATE AVERAGE AND DEVIATION: ave_f, sig_f)
	ave_f = Sbf / Nb;
	sig_f = sqrt((Sbf2 / Nb - ave_f * ave_f) / (Nb - 1));

	// accP = acc / (Nw * Nk * Nb);//?
	printf("\n accP = %4.1lf \%\n", accP * 100.0);
	printf("\n max dx = %6.4lf, %6.4lf, %6.4lf\n", dX[1], dX[2], dX[3]);
	printf("\n <r> = %8.5lf +- %8.5lf \n", ave_f, sig_f);
	fclose(ff);
	fclose(frNw);
	return 0;
}

// 0) DEFINIRAJTE (DEFINE) Psi
// double Psi(double r, double z) // za n = 2, l = 1, m = 0
// {
// 	return z * exp(-r / 2.);
// }

double Psi(double r, double z) // za n = 3, l = 0, m = 0
{
	// return pow(z, 1.5) * (27 - 18 * r + 2 * r * r) * exp(-r / 3.);
	return (27 - 18 * r + 2 * r * r) * exp(-r / 3.);
}
