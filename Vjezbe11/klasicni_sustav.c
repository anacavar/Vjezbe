/* ============================================================================================
	 _______________________ ______  __   ______________________  ___
	 __  ___/___  __/__  __ \___  / / /   __  ___/____  _/___   |/  /
	 _____ \ __  /   _  / / /__  /_/ /    _____ \  __  /  __  /|_/ /
	 ____/ / _  /    / /_/ / _  __  /     ____/ / __/ /   _  /  / /
	 /____/  /_/     \____/  /_/ /_/      /____/  /___/   /_/  /_/

			Prirodoslovno-matematicki fakultet u Splitu
			Stohasticke simulacije u klasicnoj i kvantnoj fizici
			Metropolis algoritam :: Atomi rasporedjeni po trokutastoj resetci,
															interakcija opisana L-J potencijalom
			Generator resetke: xy_trokutasta.c
			2015/16, 2017/18, 2018/19, 2019/20, 2020/21

Koristene oznake:

			Nk    = broj koraka
			ik    = indeks koraka, trenutni korak
			Nb    = broj blokova
			ib    = indeks bloka, trenutni blok
			i, j  = indeksi cestice ili pomocni indeksi
			NbSkip= broj preskocenih blokova
			NbEff = efektivni broj koraka tijekom kojih su sumirani podaci

			my2   = broj parova redaka u y smjeru (ukupno/2)
			Np    = broj atoma - povezan s my2 (trokutasta.c)

			ro    = gustoca = broj atoma / povrsina
			Lx    = sirina podrucja
			Ly    = visina podrucja
			Lxp   = polovica sirine
			Lyp   = polovica visine
			Lmin  = manja stranica, min{Lx,Ly}
			Lminp = Lmin/2
			Lminp2= Lminp*Lminp
			S     = povrsina podrucja

			x     = polozaji atoma
			dx    = maksimalna duljina pomaka
			xp    = probni polozaji atoma

			E     = energija sustava
			dE    = promjena energije pri prijelaz u probne polozaje

			gr    = razdioba udaljenosti po podrucjima
			dr    = sirina podrucja
			Skv   = povrsina podrucja (kruznog vijenca)
			rxi   = pomocna x koordinata cestice i
			ryi   = pomocna y koordinata cestice i
			rxij  = razlika x koodrdinata cestica i, j
			ryij  = razlika y koodrdinata cestica i, j
			rij2  = kvadrat udaljenosti cestica i, j
			rij2  = udaljenost cestica i, j
			ir    = pomocni indeks za sortiranje rij u odgovarajuce podrucje
			br    = pomocni brojac za normiranje

			SkF   = suma (S) po koracima  (k) srednjih vrijednosti velicine F
			SbF   = suma (S) po blokovima (b) srednjih vrijednosti velicine F
			SkF2  = suma (S) po koracima  (k) srednjih vrijednosti velicine F*F
			SbF2  = suma (S) po blokovima (b) srednjih vrijednosti velicine F*F

============================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"
#define PI 3.14159265

///////////////////////////////////////////////////////////////////////////////////////////////
//  PARAMETRI RESETKE :
// #define ro 0.08
#define ro 0.8
#define my2 3
#define Np 30

///////////////////////////////////////////////////////////////////////////////////////////////
//  PARAMETRI SIMULACIJE :
#define NbSkip 300
#define Nb 500
#define Nk 100
#define Nw 10
#define kT 0.1
#define d0 0.1

///////////////////////////////////////////////////////////////////////////////////////////////
//  PARAMETRI RAZDIOBE :
#define Ng 500 // broj grupa (dijelova) podrucja za skupljanje

double Lx, Ly, Lxp, Lyp, Lminp2;

int main()
{
	long idum = (-2);
	int i, j, ib, NbEff, ik, iw, ir, mx;
	double Acc, Acc_ib;
	double Lmin, Lminp, dmx, S, o, a;
	double x[Nw + 1][Np + 2][2], xp[Nw + 1][Np + 2][2], dx = d0;
	double E[Nw + 1], dE, SkE, SkE2, SbE, SbE2, sigmaE, Evani;
	double gr[Ng + 1], dr, rxi, ryi, rxij, ryij, rij, rij2, br = 0., Skv;
	double energija(double x[Nw + 1][Np + 2][2], int iw);

	FILE *f_xy, *f_E, *f_gr;
	f_E = fopen("E.dat", "w");
	f_gr = fopen("f_gr.dat", "w");

	///////////////////////////////////////////////////////////////////////////////////////////////
	//  ODREDJIVANJE UDALJENOSTI:
	// Atomi smjesteni u vrhovima jednakostranicnih trokuta duljine stranice a
	// Promatramo dio prostora priblizno oblika kvadrata pa priblizno vrijedi:
	//   broj stupaca * a = broj redaka * ( visina=a*sqrt(3)/2 )
	//   broj stupaca     = broj parova redaka * sqrt(3)
	dmx = (double)(my2)*sqrt(3.) + 0.5; // + 0.5 da npr. 2.8 zaokruzi na 3, a ne 2
	mx = (int)dmx;											// broj mjesta u x smjeru (broj stupaca)
	// provjera postavki
	if (Np != 2 * mx * my2)
	{ // broj atoma, 2* jer su dani parovi redaka
		printf("Nisu uskladjene postavke sa onima u kodu za generiranje resetke xy_trokutasta.c\n");
		system("PAUSE");
		return 0;
	}
	S = (double)(Np) / ro; // povrsina = broj cestica / gustoca
	// o = Lx / Ly = mx * a / (2*my2 * a*sqrt(3)/2)
	o = (double)mx / (my2 * sqrt(3.0));
	Lx = sqrt(S * o);
	Ly = sqrt(S / o);
	a = Lx / (double)(mx); // udaljenost atoma
	Lxp = Lx / 2.;
	Lyp = Ly / 2.;
	Lmin = Lx;
	if (Ly < Lx)
		Lmin = Ly;
	Lminp = Lmin / 2.;
	Lminp2 = Lminp * Lminp;
	dr = Lminp / (Ng * 1.0);
	printf("%d cestica u podrucju %lf X %lf, Lmin/2 = %f\n", Np, Lx, Ly, Lminp);

	///////////////////////////////////////////////////////////////////////////////////////////////
	//  POCETNE VRIJEDNOSTI:
	f_xy = fopen("xy.dat", "r");
	for (i = 1; i <= Np; i++)
	{
		fscanf(f_xy, " %lf %lf ", &rxi, &ryi);
		for (iw = 1; iw <= Nw; iw++)
		{
			x[iw][i][0] = rxi;
			x[iw][i][1] = ryi;
		}
	}
	fclose(f_xy);
	for (iw = 1; iw <= Nw; iw++)
		E[iw] = energija(x, iw); // pocetna energija
	printf(" - pocetna energija: %f\n", E[1]);
	Acc = 0.;
	SbE = 0.;
	SbE2 = 0.;
	for (i = 0; i <= Ng; i++)
		gr[i] = 0.;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//  SIMULACIJA :
	for (ib = 1; ib <= Nb; ib++)
	{
		SkE = 0.;
		SkE2 = 0.;
		for (ik = 1; ik <= Nk; ik++)
		{
			for (iw = 1; iw <= Nw; iw++)
			{
				for (i = 1; i <= Np; i++)
				{
					xp[iw][i][0] = x[iw][i][0] + dx * (2. * ran1(&idum) - 1.); // predlaganje pomaka setaca
					xp[iw][i][1] = x[iw][i][1] + dx * (2. * ran1(&idum) - 1.);
					if (xp[iw][i][0] < 0.)
						xp[iw][i][0] += Lx; // periodicni rubni uvjeti
					if (xp[iw][i][0] > Lx)
						xp[iw][i][0] -= Lx;
					if (xp[iw][i][1] > Ly)
						xp[iw][i][1] -= Ly;
					if (xp[iw][i][1] < 0.)
						xp[iw][i][1] += Ly;
				}
				dE = energija(xp, iw) - E[iw]; // promjena energije
				if ((dE > 0) && (exp((-dE) / kT) <= ran1(&idum)))
				{ // nemoj prihvatiti
					Acc = Acc + 1. / Nk / Nw;
					dE = 0.;
				}
				else
				{ // prihvati
					for (i = 1; i <= Np; i++)
					{
						x[iw][i][0] = xp[iw][i][0];
						x[iw][i][1] = xp[iw][i][1];
					}
				}
				E[iw] = E[iw] + dE;
				// Akumulacija vrijednosti
				if (ib > NbSkip)
				{
					SkE += E[iw] / Nw;
					SkE2 += E[iw] * E[iw] / Nw;
					for (i = 1; i < Np; i++)
					{ // petlja po parovima za g(r)
						rxi = x[iw][i][0];
						ryi = x[iw][i][1];
						for (j = i + 1; j <= Np; j++)
						{
							rxij = rxi - x[iw][j][0];
							ryij = ryi - x[iw][j][1];
							// ZAD1: primijeniti aproksimaciju min. slika
							if (rxij < -Lxp)
							{
								rxij += Lx;
							}
							if (rxij > Lxp)
							{
								rxij -= Lx;
							}
							if (ryij < -Lyp)
							{
								ryij += Ly;
							}
							if (ryij > Lyp)
							{
								ryij -= Ly;
							}
							// ZAD2: odrediti udaljenost cestica i-j: (rij2, rij)
							rij2 = rxij * rxij + ryij * ryij;
							rij = sqrt(rij2);
							// ZAD3: procijeniti podrucje (ir) i smjestiti u polje (gr)
							// ZAD4: pobrojati sve izracunate udaljenosti: (br)
							if (rij < Lminp)
							{
								ir = (int)(rij / dr);
								gr[ir]++;
								br++;
							}
						}
					}
				} // ib>NbSkip
			}		// kraj petlje setaca
		}			// kraj petlje koraka
		// Maksimalnu duljinu koraka podesavamo kako bi prihvacanje bilo oko 50%
		Acc_ib = 1. - Acc / ib;
		if (Acc_ib > 0.5)
			dx = dx * 1.05;
		if (Acc_ib < 0.5)
			dx = dx * 0.95;
		if (ib > NbSkip)
		{
			SbE += SkE / Nk;
			SbE2 += SkE2 / Nk;
			NbEff = ib - NbSkip;
			if (NbEff == 1)
				sigmaE = 0.0;
			else
				sigmaE = sqrt(SbE2 / NbEff - SbE * SbE / NbEff / NbEff) / sqrt(NbEff - 1.);
			fprintf(f_E, "%d\t%lf\t%lf\t%lf\n", ib, SkE / Nk, SbE / NbEff, sigmaE);
		} // kraj if petlje (ib>NbSkip)
	}		// kraj petlje blokova

	// ZAD5: NORMIRATI I ISPISATI RADIJALNU DISTRIBUCIJSKU FUNKCIJU
	for (ir = 1; ir < Ng; ir++)
	{
		fprintf(f_gr, "%lf\t%lf\n", ir * dr, gr[ir] / (2 * br * M_PI * ir * dr * dr));
	}

	printf(" - postotak prihvacenih pomaka: %lf\n", Acc_ib * 100.0);
	Evani = 4. * Np * PI * ro * (0.2 / pow(Lminp, 10.) - 0.5 / pow(Lminp, 4.));
	printf(" - energija unutar Lmin/2: %lf\n", SbE / NbEff);
	printf(" - energija van Lmin/2: %lf\n", Evani);
	fclose(f_E);
	fclose(f_gr);
	return 0;
}

/* racuna energiju sustava */
double energija(double x[Nw][Np + 2][2], int iw)
{
	int i, j;
	double rxi, ryi, rxij, ryij, rij2;
	double sum, sr2, sr6, sr12;
	sum = 0.;
	for (i = 1; i < Np; i++)
	{
		rxi = x[iw][i][0];
		ryi = x[iw][i][1];
		for (j = i + 1; j <= Np; j++)
		{
			rxij = rxi - x[iw][j][0];
			if (rxij > Lxp)
				rxij = rxij - Lx;
			if (rxij < -Lxp)
				rxij = rxij + Lx;
			ryij = ryi - x[iw][j][1];
			if (ryij > Lyp)
				ryij = ryij - Ly;
			if (ryij < -Lyp)
				ryij = ryij + Ly;
			rij2 = rxij * rxij + ryij * ryij;
			if (rij2 <= Lminp2)
			{
				sr2 = 1. / rij2;
				sr6 = sr2 * sr2 * sr2;
				sr12 = sr6 * sr6;
				sum += sr12 - sr6;
			}
		}
	}
	return (4. * sum);
}
