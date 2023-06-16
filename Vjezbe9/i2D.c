/* ============================================================================================
	 _______________________ ______  __   ______________________  ___
	 __  ___/___  __/__  __ \___  / / /   __  ___/____  _/___   |/  /
	 _____ \ __  /   _  / / /__  /_/ /    _____ \  __  /  __  /|_/ /
	 ____/ / _  /    / /_/ / _  __  /     ____/ / __/ /   _  /  / /
	 /____/  /_/     \____/  /_/ /_/      /____/  /___/   /_/  /_/

			Prirodoslovno-matematicki fakultet u Splitu
			Stohasticke simulacije u klasicnoj i kvantnoj fizici
			Metropolis algoritam :: Isingov 2D model
			Petar Stipanovic, nastavni materijal
			2016/2017, 2019/2020, 2020/21

Koristene oznake:

			Nk    = broj koraka (number of steps)
			ik    = indeks koraka, trenutni korak
			Nb    = broj blokova (number of blocks)
			ib    = indeks bloka, trenutni blok
			NbSkip= broj preskocenih blokova
			NbEff = efektivni broj koraka tijekom kojih su sumirani podaci

			s     = 2D polje spinova
			i,j   = indeksi spina, odabrani element
			max   = L, dimenzija resetke LxL

			E     = energija trenutne konfiguracije
			M     = magnetizacija trenutne konfiguracije
			dE    = promjena energije
			dM    = promjena magnetizacije
			C     = toplinski kapacitet
			sus   = magnetska susceptibilnost

			SkF   = suma (S) po koracima  (s, steps)  srednjih vrijednosti velicine F
			SbF   = suma (S) po blokovima (b, blocks) srednjih vrijednosti velicine F
			SkF2  = suma (S) po koracima  (s, steps)  srednjih vrijednosti velicine F*F
			SbF2  = suma (S) po blokovima (b, blocks) srednjih vrijednosti velicine F*F

			aF  = prosjek velicine F   (average value of F)
			aF2 = prosjek velicine F*F (average value of F*F)

			Jedinice biramo tako da je vrijednost spina 1 ili -1 te k = 1 pa je u kodu kT = T!

============================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ran1.c"

#define max 4		// dimenzija resetke max X max
#define kT0 1.0 // pocetna temperatura
#define dkT 0.1 // temperaturni korak
#define nT 40		// broj razlicitih temperatura
#define Jv 1.0	// konstanta izmjene energije

#define NbSkip 1000
#define Nk 10000
#define Nb 11000

#define write 0 // pohrana mape spinova (0=NE, 1=DA)

int main()
{
	long idum = (-25879);
	int it, ib, ik, NbEff, s[max + 2][max + 2], i, j;
	double E, dE, sigmaE, M, dm, sigmaM, dM;
	double SkE, SkE2, SbE, SbE2, aE, aE2;
	double SkM, SkM2, SbM, SbM2, aM, aM2;
	double kT, C, sus, reject, X;
	double magnetizacija(int s[max + 2][max + 2]); // magnetizacija sustava
	double energija(int s[max + 2][max + 2]);			 // energija sustava

	FILE *fsG, *fsD, *fm, *fE, *fT, *fmx;
	fm = fopen("f_m.dat", "w");
	fprintf(fm, "#b, Mb, M, sigM\n");
	fE = fopen("f_E.dat", "w");
	fprintf(fE, "#b, Eb, E, sigE\n");
	fT = fopen("f_T.dat", "w");
	fprintf(fT, "#T, c, x\n");
	fsG = fopen("f_spinG.dat", "w");
	fsD = fopen("f_spinD.dat", "w");
	fmx = fopen("f_TmX.dat", "w");

	// Pocetna konfiguracija
	for (i = 1; i <= max; i++)
	{
		for (j = 1; j <= max; j++)
		{
			s[i][j] = 1;
			printf("%d\t%d\t%d\n", i, j, s[i][j]);
		}
	}

	for (it = 0; it <= nT; it++)
	{
		kT = kT0 + it * dkT;
		SbE = 0.;
		SbM = 0.;
		SbE2 = 0.;
		SbM2 = 0.;
		reject = 0;
		// pocetna energija i magnetizacija
		E = energija(s);
		// ZAD1: doraditi funkciju da racuna pocetnu magnetizaciju
		M = magnetizacija(s);
		printf("T = %e K\n - pocetak:\n   E = %e\n   M = %e\n", kT, E, M);
		for (ib = 1; ib <= Nb; ib++)
		{ // Petlja po blokovima
			SkE = 0.;
			SkM = 0.;
			SkE2 = 0.;
			SkM2 = 0.;

			for (ik = 1; ik <= Nk; ik++)
			{																		 // Petlja po koracima
				i = (int)(1. + ran1(&idum) * max); // izaberi element od 1 do max
				j = (int)(1. + ran1(&idum) * max);
				s[i][j] *= -1; // promjena spina
				// periodicni rubni uvjeti, mijenjaju se samo kada zatrebaju
				if (i == 1)
					s[i - 1][j] = s[max][j];
				if (i == max)
					s[i + 1][j] = s[1][j];
				if (j == 1)
					s[i][j - 1] = s[i][max];
				if (j == max)
					s[i][j + 1] = s[i][1];
				dE = -2. * Jv * s[i][j] * (s[i + 1][j] + s[i - 1][j] + s[i][j + 1] + s[i][j - 1]); // promjena energije
				// ZAD2: definirati promjenu magnetizacije i pravilno je odbaciti
				dM = 2. * s[i][j]; // promjena magnetizacije
				if ((dE > 0) && (exp((-dE) / kT) <= ran1(&idum)))
				{
					s[i][j] = s[i][j] * (-1);				// odbaci promjenu
					reject = reject + 1. / Nk / Nb; // postotak odbacenih koraka
					dE = 0;
					dM = 0; // odbacivanje promjene
				}
				E = E + dE;
				// ZAD3: izracunati magnetizaciju na osnovu prihvacene/neprihvacene promjene
				M = M + dM;

				// ZAD4: sakupljati i uporosjeciti magnetizaciju
				//       (resetirati i dodati velicine gdje god je potrebno u kodu)

				if (ib > NbSkip)
				{
					SkE += E;
					SkM += M;
					SkE2 += E * E;
					SkM2 += M * M;
				}
			} // kraj petlje koraka
			if (ib > NbSkip)
			{
				SbE += SkE / Nk;
				SbM += SkM / Nk;
				SbE2 += SkE2 / Nk;
				SbM2 += SkM2 / Nk;

				// Pohrama mape spinova u odvojenim datotekama
				if (write == 1)
				{
					for (i = 1; i <= max; i++)
					{
						for (j = 1; j <= max; j++)
						{
							if (s[i][j] == 1)
								fprintf(fsG, "%d  %d\n", i, j);
							if (s[i][j] == -1)
								fprintf(fsD, "%d  %d\n", i, j);
						}
					}
					fprintf(fsG, "\n\n");
					fprintf(fsD, "\n\n");
				}
				NbEff = ib - NbSkip;
				aE = SbE / NbEff;
				aE2 = SbE2 / NbEff;
				sigmaE = sqrt(aE2 - aE * aE) / sqrt(NbEff);
				fprintf(fE, "%d  %e  %e  %e\n", ib, SkE / Nk, aE, sigmaE);

				aM = SbM / NbEff;
				aM2 = SbM2 / NbEff;
				sigmaM = sqrt(aM2 - aM * aM) / sqrt(NbEff);
				fprintf(fm, "%d  %e  %e  %e\n", ib, SkM / Nk, aM, sigmaM);

			} // kraj if petlje (ib>NbSkip)
		}		// kraj petlje po blokovima
		// ZAD5: dodatno pohraniti kT, magnetizaciju i susceptibilnost
		X = (aM2 - aM * aM) / kT;
		fprintf(fmx, "%e  %e  %e\n", kT, aM / (max * max), X / (max * max));

		C = (aE2 - aE * aE) / (kT * kT);
		fprintf(fT, "%e  %e\n", kT, C / (max * max));
		fprintf(fE, "\n\n");
		printf(" - kraj:\n   <E> = %e\n   <M> = %e\n", aE, aM);
		printf(" - prihvacenost %e  = \n", (1. - reject) * 100);
		if (write == 1)
			printf(" - konacni raspored spinova: f_spinG.dat i f_spinD.dat\n");
		fprintf(fE, "\n\n");
		fprintf(fm, "\n\n");
	}
	fclose(fm);
	fclose(fE);
	fclose(fmx);
	fclose(fT);
	fclose(fsG);
	fclose(fsD);
	return 0;
}

double energija(int s[max + 2][max + 2])
{
	int i, j, gore, desno;
	double sum;
	sum = 0.;
	for (i = 1; i <= (max); i++)
	{
		for (j = 1; j <= max; j++)
		{
			if (i == max)
				desno = 1;
			else
				desno = i + 1;
			if (j == max)
				gore = 1;
			else
				gore = j + 1;
			sum += s[i][j] * (s[i][gore] + s[desno][j]);
		}
	}
	return (-Jv * sum);
}
double magnetizacija(int s[max + 2][max + 2])
{
	int i, j;
	double sum;
	sum = 0.;
	// ZAD1: pohraniti magnetizaciju u sum
	for (i = 1; i <= (max); i++)
	{
		for (j = 1; j <= max; j++)
		{
			sum += s[i][j];
		}
	}

	return sum;
}
