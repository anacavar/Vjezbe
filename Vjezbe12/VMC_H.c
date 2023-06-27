/* ====================================================================
	 _______________________ ______  __   ______________________  ___
	 __  ___/___  __/__  __ \___  / / /   __  ___/____  _/___   |/  /
	 _____ \ __  /   _  / / /__  /_/ /    _____ \  __  /  __  /|_/ /
	 ____/ / _  /    / /_/ / _  __  /     ____/ / __/ /   _  /  / /
	 /____/  /_/     \____/  /_/ /_/      /____/  /___/   /_/  /_/
			Prirodoslovno-matematicki fakultet u Splitu
			Stohasticke simulacije u klasicnoj i kvantnoj fizici
			Varijacijski Monte Carlo :: Osnovno stanje vodikova atoma
			2018/2019, 2019/2020
Koristene oznake:
			Nw    = broj setaca (number of walkers)
			iw    = indeks setaca (index of walker)
			Nb    = broj blokova (number of blocks)
			ib    = indeks bloka (index of block)
			Nt    = broj koraka (number of time-steps)
			it    = indeks vremenskog koraka (time-step)
			k     = koordinata (coordinate)
			NbSkip= broj preskocenih blokova
			E     = lokalna energija (local energy)
			SwE   = suma (srednjih E) po setacima
			StE   = suma (srednjih E) po koracima
			SbE   = suma (srednjih E) po blokovima
			accept= brojac prihvacenih koraka
			acc_ib= udio prihvacenih koraka
			x     = koordinate polozaja setaca
			dx    = promjena koordinate nasumicno od -dk do dk
			dk    = maksimalna duljina koraka
			xp    = koordinate probnog polozaja setaca
			r1    = modul radijvektora probnog polozaja
			r2    = r1*r1
			P     = vjerojatnost nalazenja na polozaju x
			Pp    = vjerojatnost nalazenja u probnom polozaju
			T     = vjerojatnost prijelaza x -> xp
			Psi   = valna funkcija
======================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define Nt 500		// broj koraka
#define Nw 500		// broj setaca
#define Nb 210		// broj blokova
#define NbSkip 10 // broj prvih blokova koje preskacemo
#define b 1.20		// parametar probne valne funkcije

int main()
{
	long idum = (-212);
	int it, ib, iw, k, NbEff, itmp;
	double accept, acc_ib, AE, sigmaE;
	double x[4][Nw + 1], xp[4], dk[4], E[Nw + 1], P[Nw + 1], Psi(double, double);
	double dx, r1, r2, SwE, StE, SbE, SbE2, Pp, T;
	FILE *fout;
	accept = 0.;								 // prihvacanje
	dk[1] = dk[2] = dk[3] = 1.5; // maksimalne promjene koordinata
	fout = fopen("E.dat", "w");	 // datoteka za pohranu srednjih vrijednosti
	// inicijalizacija polozaja gdje je gustoca Psi*Psi znacajna
	for (iw = 1; iw <= Nw; iw++)
	{
		r2 = 0.;
		for (k = 1; k <= 3; k++)
		{
			x[k][iw] = 15. * (ran1(&idum) - 0.5);
			r2 = r2 + x[k][iw] * x[k][iw];
		}
		r1 = sqrt(r2);
		P[iw] = Psi(r1, x[3][iw]) * Psi(r1, x[3][iw]);
		E[iw] = 2. / r1 * (b - 1.) - b * b;
	}
	SbE = 0.;
	SbE2 = 0.;
	for (ib = 1; ib <= Nb; ib++)
	{ // blokovi
		StE = 0.;
		NbEff = ib - NbSkip;
		for (it = 1; it <= Nt; it++)
		{ // koraci
			SwE = 0;
			for (iw = 1; iw <= Nw; iw++)
			{ // setaci
				r2 = 0.;
				for (k = 1; k <= 3; k++)
				{ // koordinate
					dx = 2. * (ran1(&idum) - 0.5) * dk[k];
					xp[k] = x[k][iw] + dx;
					r2 = r2 + xp[k] * xp[k];
				}
				r1 = sqrt(r2);
				Pp = Psi(r1, xp[3]) * Psi(r1, xp[3]);
				T = Pp / P[iw];
				// Metropolis algoritam
				if (T >= 1)
				{
					for (k = 1; k <= 3; k++)
					{
						x[k][iw] = xp[k];
					}
					accept += 1.;
					P[iw] = Pp;
					E[iw] = 2. / r1 * (b - 1.) - b * b;
				}
				else if (ran1(&idum) <= T)
				{
					for (k = 1; k <= 3; k++)
					{
						x[k][iw] = xp[k];
					}
					accept += 1.;
					P[iw] = Pp;
					E[iw] = 2. / r1 * (b - 1.) - b * b;
				}
				SwE = SwE + E[iw];
			} // setaci
			// akumulacija podataka nakon stabilizacije
			if (ib > NbSkip)
			{
				StE += SwE / Nw;
			}
		} // koraci
		// maksimalnu duljinu koraka podesavamo kako bi prihvacanje bilo 50%
		acc_ib = accept / (ib * Nw * Nt);
		if (acc_ib > 0.5)
			for (k = 1; k <= 3; k++)
				dk[k] = dk[k] * 1.05;
		if (acc_ib < 0.5)
			for (k = 1; k <= 3; k++)
				dk[k] = dk[k] * 0.95;
		if (ib > NbSkip)
		{
			SbE += StE / Nt;
			SbE2 += StE * StE / (Nt * Nt);
			fprintf(fout, "%7d%16.8e%16.8e\n", NbEff, StE / Nt, SbE / NbEff);
		}
		itmp = (int)(round(acc_ib * 100.));
		printf("%6d. blok:  %d%% prihvacenih,  Eb = %10.2e\n", NbEff, itmp, StE / Nt);
	} // blokovi
	AE = SbE / NbEff;
	sigmaE = sqrt((SbE2 / NbEff - AE * AE) / (NbEff - 1.));
	accept = accept / (Nw * Nt * Nb);
	printf("\n konacni max. koraci: %6.2e %6.2e %6.2e\n\n", dk[1], dk[2], dk[3]);
	printf("\n E = %8.5e +- %6.2e \n\n", AE, sigmaE);
	fclose(fout);
	return 0;
}

double Psi(double r, double z)
{
	return exp(-b * r);
	// return exp(-pow(b / r, 5) - s * r);
}
