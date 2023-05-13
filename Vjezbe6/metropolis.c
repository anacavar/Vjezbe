/* ============================================================================================
	 _______________________ ______  __   ______________________  ___
	 __  ___/___  __/__  __ \___  / / /   __  ___/____  _/___   |/  /
	 _____ \ __  /   _  / / /__  /_/ /    _____ \  __  /  __  /|_/ /
	 ____/ / _  /    / /_/ / _  __  /     ____/ / __/ /   _  /  / /
	 /____/  /_/     \____/  /_/ /_/      /____/  /___/   /_/  /_/

			Prirodoslovno-matematicki fakultet u Splitu
			Stohasticke simulacije u klasicnoj i kvantnoj fizici
			Metropolis algoritam :: <r> za Vodikov atom u stanju |2,1,0>
			2011/2012, 2015/2016, 2017/18, 2018/2019, 2019/20, 2020/21

Koristene oznake:

			Nb    = broj blokova (number of blocks)
			ib    = indeks bloka, trenutni blok
			Nk    = broj koraka (number of steps)
			ik    = indeks koraka, trenutni korak
			Nw    = broj setaca (nuber of walkers)
			iw    = indeks setaca
			NbSkip= broj preskocenih blokova

			f     = vrijednost funkcije f
			Swf   = zbroj vrijednosti od f po svim setacima
			Skf   = zbroj srednjih vrijednosti po koracima
			Sbf   = zbroj srednjih vrijednosti po blokovima

			acc   = brojac prihvacenih koraka
			accP  = udio prihvacenih koraka
			Nacc  = broj koraka za provjeru prihvacanja

			x     = koordinate polozaja setaca
			dx    = promjena koordinate
			dX    = maksimalna duljina koraka
			xp    = koordinate probnog polozaja setaca
			rp    = r probnog polozaja
			P     = vjerojatnot nalazenja na polozaju x
			Pp    = vjerojatnost nalazenja u probnom polozaju
			T     = vjerojatnost prijelaza x -> xp
			Psi   = valna funkcija

============================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define NbSkip 100 // koliko prvih blokova preskociti
#define Nb 1000		 // broj blokova (uzetih u obzir)
#define Nk 100		 // broj koraka
#define Nw 5000		 // broj setaca
#define Nacc 100	 // provjera prihvacanja

main(void)
{
	long idum = (-2123);
	int ik = 0, ib = 1, iw, k;
	double acc, accP, ave_f, sig_f;
	double x[4][Nw + 1], xp[4], dX[4], f[Nw + 1], P[Nw + 1], Psi(double);
	double dx, rp, rp2, Swf, Skf, Sbf, Sbf2, Pp, T, tmp;
	FILE *ff, *frNw, *frNw0;

	dX[1] = dX[2] = dX[3] = 5.;						// Maksimalne promjene koordinata
	ff = fopen("r.dat", "w");							// Datoteka za pohranu srednjih vrijednosti
	frNw = fopen("rNw_100Nk++.dat", "w"); // Datoteka za pohranu polozaja
	frNw0 = fopen("rNw_0.dat", "w");			// Datoteka za pohranu polozaja

	// Inicijalizacija polozaja prema Psi*Psi
	for (iw = 1; iw <= Nw; iw++)
	{
		rp2 = 0.;
		for (k = 1; k <= 3; k++)
		{
			x[k][iw] = 50. * (ran1(&idum) - 0.5);
			rp2 = rp2 + x[k][iw] * x[k][iw];
		}
		fprintf(frNw0, "%7d  %13.8e  %13.8e  %13.8e\n", (ib - 1) * Nk + ik, x[1][iw], x[2][iw], x[3][iw]);
		rp = sqrt(rp2);
		tmp = Psi(rp);
		P[iw] = tmp * tmp;
		f[iw] = rp;
	}
	fclose(frNw0);
	acc = 0.;
	Sbf = 0.;
	Sbf2 = 0.;
	for (ib = 1 - NbSkip; ib <= Nb; ib++)
	{ // blokovi
		Skf = 0.;
		if (ib == 1)
			acc = 0.;
		for (ik = 1; ik <= Nk; ik++)
		{ // koraci
			Swf = 0;
			for (iw = 1; iw <= Nw; iw++)
			{ // setaci
				rp2 = 0.;
				for (k = 1; k <= 3; k++)
				{ // koordinate
					dx = 2. * (ran1(&idum) - 0.5) * dX[k];
					xp[k] = x[k][iw] + dx;
					rp2 = rp2 + xp[k] * xp[k];
				}
				rp = sqrt(rp2);
				tmp = Psi(rp);
				Pp = tmp * tmp;
				T = Pp / P[iw];
				if (T >= 1)
				{
					for (k = 1; k <= 3; k++)
					{
						x[k][iw] = xp[k];
					}
					acc += 1.;
					P[iw] = Pp;
					f[iw] = rp;
				}
				else if (ran1(&idum) <= T)
				{
					for (k = 1; k <= 3; k++)
					{
						x[k][iw] = xp[k];
					}
					acc += 1.;
					P[iw] = Pp;
					f[iw] = rp;
				}
				Swf = Swf + f[iw];
				// 1) POHRANITI SVE POLOZAJE U ZADNJEM KORAKU
				// SVAKOG 100-TOG BLOKA (moze u frNw)
				if (ib % 100 == 0 && ik == Nk)
				{
					for (iw = 1; iw <= Nw; iw++)
					{
						fprintf(frNw, "%7d  %13.8e  %13.8e  %13.8e\n", (ib - 1) * Nk + ik, x[1][iw], x[2][iw], x[3][iw]);
					}
				}

			} // setaci
			// Maksimalnu duljinu koraka podesavamo kako bi prihvacanje bilo oko 50%
			if (((ib - 1 + NbSkip) * Nk + ik) % Nacc == 0 && ib < 1)
			{ // tijekom stabilizacije
				accP = acc / (Nacc * Nw);
				// 2) PRILAGODITI KOORDINATNE POMAKE
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
					printf("blok: %d, prihvacenost: %3.1lf \n", ib, accP * 100.);
				acc = 0.0;
			}
			else if (ib > 0)
			{ // nakon stabilizacije
				accP = acc / ((ib - 1.0) * Nw * Nk + ik * Nw);
				// 3) PRILAGODITI KOORDINATNE POMAKE
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
			// Akumulacija podataka nakon stabilizacije
			if (ib > 0)
			{
				Skf += Swf / Nw;
				if (ib % 100 == 0 && ik == Nk)
					fprintf(frNw, "\n\n");
			}
		} // koraci

		if (ib > 0)
		{
			Sbf += Skf / Nk;
			Sbf2 += Skf * Skf / (Nk * Nk);
			fprintf(ff, "%7d  %13.8e  %13.8e\n", ib, Skf / Nk, Sbf / ib);
		}
		if (ib % 100 == 0)
			printf("blok: %d, prihvacenost: %3.1lf \n", ib, accP * 100.);
	} // blokovi

	// 4) IZRECUNAJTE PROSJECNU VRIJEDNOST ave_f I DEVIJACIJU sig_f te PRIHVACENOST
	ave_f = Sbf / Nb;
	sig_f = sqrt(((Sbf2 / Nb) - ave_f * ave_f) / (Nb - 1));

	accP = acc / (Nw * Nk * Nb);
	printf("Postotak prihvacenih koraka: %4.1lf\n", accP * 100.0);
	printf("\nKonacne maksimalne duljine koraka: %6.4lf %6.4lf %6.4lf\n", dX[1], dX[2], dX[3]);
	printf("\nSrednja udaljenost: %8.5lf +- %8.5lf \n", ave_f, sig_f);
	fclose(ff);
	fclose(frNw);
}

// 0) DEFINIRAJTE Psi

// double Psi (double r, double z){
//    return z*exp(-r/2.);
// }

double Psi(double r)
{
	return ((27. - 18. * r + 2. * r * r) * exp(-r / 3.));
}
