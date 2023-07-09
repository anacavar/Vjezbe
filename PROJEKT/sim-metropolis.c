// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define N 10 // broj čestica
#define Nw 5 // broj šetača
// #define Nw 1000 // broj šetača
#define Nk 100 // broj koraka
// #define Nk 1000 // broj koraka
// #define Nb 100  // broj blokova

float lennardJones(float x1, float x2, float y1, float y2)
{
  // float E = 1.65 * pow(10, -21); // J, za Argon
  float E = 1; // J, za Argon
  float r = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
  // float sigma = 3.4 * pow(10, -10); // m, za Argon
  float sigma = 1; // m, za Argon
  float a;
  // printf("%f\n", r);
  if (r >= 0.99 * sigma) // jednostavno ignoriraj parove koji su preblizu tako da divergiraju - KOLKO SIGMI TU?
    a = 4 * E * (pow(sigma / r, 12) - pow(sigma / r, 6));
  else
    a = 0;
  return a;
}

// računa W = šta ono, onaj eksponent raspodjele
float funkcija(float x[Nw + 1][N + 1], float y[Nw + 1][N + 1], int i)
{
  float potEtemp = 0;
  int a, b;
  for (a = 0; a < N; a++)
  {
    for (b = a + 1; b < N; b++)
    {
      potEtemp += lennardJones(x[i][a], x[i][b], y[i][a], y[i][b]);
    }
  }
  // potEnergy[i] = potEtemp;
  return potEtemp;
}

int main(void)
{

  float m = 6.69 * pow(10, -26); // kg, za Argon

  int i, j, k;
  int a, b;
  long idum = -1234;
  float E0 = 20.; // ukupna početna energija
  float p;        // pressure
  float T;        // temperature
  float L0 = 10;  // početna duljina brida spremnika L
  int ip;         // indeks randomly odabrane čestice
  // int ib;  // indeks bloka

  float x[Nw + 1][N + 1]; // broj šetača, broj čestica
  float y[Nw + 1][N + 1]; // broj šetača, broj čestica

  float Utemp[Nw + 1];
  float Unew;
  float deltaU;
  float pp;       // vjerojatnost prihvaćanja promjene
  float beta = 1; // Boltzmannova konstanta

  float x0, y0, v0;
  // float Lx, Ly;
  float L[Nw + 1];
  float V[Nw + 1];
  float dL, dLMax = 0.5;
  float dx, dy; // promjene koordinata x i y
  float dxMax = 0.1, dyMax = 0.1;

  FILE *data, *koordinate;
  data = fopen("data.txt", "w");
  koordinate = fopen("koordinate.txt", "w");

  float postotak_prihvacanja = 0;

  // inicijalizacija čestica
  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      // dobro je da su sveˇcestice na poˇcetku udaljene barem za σ.
      x0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      y0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      x[i][j] = x0;          // broj šetača, broj čestica
      y[i][j] = y0;          // broj šetača, broj čestica
      // JER GLEDAMO SAMO UNUTARNJU ENERGIJU!!! SRC: WIKIPEDIA: https://en.wikipedia.org/wiki/Isothermal%E2%80%93isobaric_ensemble
      // fprintf(koordinate, "%f\t%f\n", x0, y0);
    }
    // računamo unutarnju energiju početne konfiguracije
    Utemp[i] = funkcija(x, y, i);
    // printf("%f\n", Utemp[i]);
    L[i] = L0;
    V[i] = L0 * L0;
  }

  for (i = 1; i <= Nw; i++) // po šetačima - mikrostanja - šetaći po 3N dimenzionalnom faznom prostoru
  {
    // printf("%d\n", i);
    for (j = 1; j <= Nk; j++) // po koracima // ZA SVAKI KORAK PROVJERITI DA LI SE PRIHVAĆA - MOŽDA PRVO ŠETAĆI PA OVO? ILI OBRNUTO?
    {
      for (k = 1; k <= N; k++) // po česticama
      {
        dx = (ran1(&idum) * 2 - 1) * dxMax;
        dy = (ran1(&idum) * 2 - 1) * dyMax;
        x[i][k] += dx; // broj šetača, broj čestica
        y[i][k] += dy; // broj šetača, broj čestica

        // rubni uvjeti
        if (x[i][k] < 0)
          x[i][k] = -x[i][k];
        if (x[i][k] > L0)
          x[i][k] = L0 - (x[i][k] - L0);
        if (y[i][k] < 0)
          y[i][k] = -y[i][k];
        if (y[i][k] > L0)
          y[i][k] = L0 - (y[i][k] - L0);
      }
      // mislim da ovdje ide Metropolis alg promjena

      // SVAKI N-ti korak promijeni i volumen...
      if (j % 5 == 0) // recimo svaki 5-ti korak ili 50ti
      {
        // printf("%d\n", j);
        dL = (ran1(&idum) * 2 - 1) * dLMax;
        L[i] = L[i] + dL;
        V[i] = L[i] * L[i];
        // printf("%f\n", V[i]);
        // printf("%f\n", L[i]);
        // trebamo skalirati promjene na koordinate čestica te pak računati U
        for (k = 1; k <= N; k++) // po česticama
        {
          x[i][k] = L[i]
        }
      }
      else
      {
        Unew = funkcija(x, y, i);
        deltaU = Unew - Utemp[i];
        // printf("prije: %f\n", Utemp[i]);
        Utemp[i] = Unew;
        // printf("poslije: %f\n", Utemp[i]);
        // printf("TROKUTU: %f\n", deltaU);

        // vjerojatnost prihvaćanja promjene
        pp = exp(-beta * deltaU);
        // printf("p je: %f\n", pp);

        // primorac Tomislav 17
      }
    }
  }

  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      fprintf(data, "%f\t%f\n", x[i][j], y[i][j]);
    }
  }

  fclose(data);
  fclose(koordinate);

  return 0;
}

// 1. Uspostavimo poˇcetno mikrostanje.
// 2. Napravimo sluˇcajnu probnu promjenu u mikrostanju. Primjerice, izaberemo neki
// spin na sluˇcajan naˇcin i preokrenemo ga.
// 3. Zatim izraˇcunamo promjenu energije zbog probne promjene mikrostanja, ∆E ≡
// Eproba −Estara.
// 4. Ako je ∆E manja ili jednaka nuli, prihvatimo probnu promjenu i prelazimo na 8.
// korak.
// 5. Ako je ∆E ve ́ca od nule, izraˇcunamo veliˇcinu w = e−β∆E.
// 6. Generiramo sluˇcajni broj r u jediniˇcnom intervalu.
// 7. Ako je r ≤ w prihva ́camo novo mikrostanje; u suprotnom odbacujemo probni
// pomak i zadržavamo staro stanje.
// 8. Odredimo vrijednosti željenih fizikalnih veliˇcina.
// 9. Ponavljamo korake (2) do (8) kako bismo skupili dovoljno mikrostanja.
// 10. Periodiˇcno raˇcunamo prosjeke preko mikrostanja.
// Koraci 2 do 7 daju uvjetnu vjerojatnost da je sustav u mikrostanju {sj},
// ako je u prethodnom koraku bio u mikrostanju {si}
// trebamo uzorkovati koordinate r1,r2,..,rN čestica i volumen sustava V .

// U NPT ansamblu, vjerojatnost mikrostanja je proporcionalna sa e−(E+PV )
// S obzirom da je tlak fiksan, probna konfiguracija može se generirati iz trenutne konfiguracije ili
// slučajnim pomakom čestice ili slučajnom promjenom volumena, primjerice V →V +δ(2r−1),
// gdje je r slučajno izabran iz jednolike funkcije razdiobe na jediničnom intervalu, a δ je maksi-
// malna promjena volumena. Probna konfiguracija se prihvaca ukoliko je ∆W ≤ 0 i s vjerojat-
// nosti e−∆W ukoliko je ∆W > 0. Nije nužno, a ni efikasno mijenjati volumen nakon svakog
// Monte Carlo koraka po čestici

// Vrijednosti udaljenosti i energije u
// tipiˇcnim teku ́cinama za koje je prikladno koristiti Lennard-Jones model interakcije vrlo su male
// u SI jedinicama. Stoga se najˇceš ́ce biraju σ i  kao jedinice za udaljenost i energiju. Tako  ̄der se
// bira za jedinicu mase masa jednog atoma m i zatim se sve druge veliˇcine izražavaju preko σ, i
// m. Primjerice, za argon je σ = 3.4 ×10−10 m, e = 1.65 ×10−21 J i m = 6.69 ×10−26 kg.
// {
//   float potE;
//   int i, j;
//   for (i = 0; i < N; i++)
//   {
//     for (j = 0; j < N; j++)
//     {
//       if (i != j)
//       {
//         printf("%f\n", x[indeksWalkera][i]);
//         printf("%f\n", x[indeksWalkera][j]);
//         potE += lennardJones(x[][indeksWalkera]);
//       }
//     }
//   }
//   return potE;
// }

// for (ib = 1; ib <= Nb; ib++) // po bloku
// {
// po blokovima za usrednjavanje
// novi položaj određujemo uvijek na osnovu prethodnog pa su dobiveni
// podaci u nekoj mjeri korelirani
// kako bismo izbjegli utjecaj korelacija na proračun standardne devijacije
// i sličnih veličina, podatke ćemo blokirati.
// Blokiranje provodimo dijeleći cjelokupnu simulaciju na blokove gdje
// svaki blok sadrži nekoliko (100, 1000, 5000, ...) koraka
// nakon svakog bloka pohranjujemo:
// ▶ indeks bloka ib
// ▶ ⟨f⟩ - usrednjenu vrijednost veličine f koju smo akumulirali od početka
// cijele simulacije
// ▶ ⟨f⟩b - usrednjenu vrijednost veličine f koju smo akumulirali samo
// unutar jednog bloka
// [12:58, 09 / 07 / 2023] אנה: // računamo potencijalnu energiju
// potEtemp = 0;
// for (a = 0; a < N; a++)
// {
//   for (b = a + 1; b < N; b++)
//   {
//     printf("a=%d, i=%d, %f\n", a, i, x[i][a]);
//     printf("%f\n", lennardJones(x[i][a], x[i][b], y[i][a], y[i][b]));
//     potEtemp += lennardJones(x[i][a], x[i][b], y[i][a], y[i][b]);
//   }
// }

// printf("%f\n", potE / 2);

// 3. Izraˇcunamo ∆E, promjenu potencijalne energije sustava zbog probnog pomaka.
// 4. Ako je ∆E ≤0 nova konfiguracija se prihva ́ca i odlazi se na 8. korak
// 5. Ako je ∆E > 0, raˇcunamo w = e−β∆E.
// 6. Generiramo iz jedolike raspodjele sluˇcajni broj r u segmentu [0,1].
// 7. Ako je r ≤ w, prihva ́camo probni pomak; u suprotnom zadržavamo prethodno
// mikrostanje.
// 8. Određujemo željene fizikalne veliˇcine.
// 9. Ponavljamo korake (2) do (8) kako bismo dobili dovoljan broj mikrostanja.
// 10. Sakupljamo podatke koji su nam potrebni za prosjeke traženih veliˇcina

// računaš novo neš i provjeravaš jel random broj spada u to neš...

// vjerojatnost mikrostanja u NPT ansamblu proporcionalna je s e−(E(i)+PV ) // E JE SAMO UNUTARNJA ENRGIJA ACCORDING TO WIKIPEDIA!!
// e−W = e−βPV −βU({s})+N ln V

// postotak_prihvacanja++;
// [13:08, 09/07/2023] אנה: Najˇceš ́ce je potrebno mnogo uzoraka sluˇcajnog hoda prije postizanja asimptotske raspod-
// jele vjerojatnosti p(x). Postavlja se pitanje kako izabrati maksimalnu veliˇcinu koraka δ. Ako
// je δ prevelik, samo  ́ce mali broj probnih koraka biti prihva ́cen pa  ́ce uzorkovanje od p(x) biti
// neefikasno. S druge strane, ako je δ premalen, veliki postotak probnih koraka bit  ́ce prihva ́cen
// pa  ́ce opet uzorkovanje p(x) biti neefikasno. Grubi kriterij za odabir veliˇcine koraka δ je da ot-
// prilike jedna tre ́cina do jedna polovica koraka trebaju biti prihva ́ceni. Tako  ̄der je dobro izabrati
// i x0 tako da raspodjela xi dostigne asimptotsku raspodjelu ˇcim je prije mogu ́ce. Oˇciti je izbor
// poˇceti sluˇcajni hod na vrijednosti od x za koju je p(x) znaˇcajan
// [13:08, 09/07/2023] אנה: 1. Izabere se probni položaj xproba = xi+ δi, gdje je δi sluˇcaji broj iz jednolike raspod-
// jele na segmentu [−δ,δ].
// 2. Izraˇcuna se w = p(xproba)/p(xi).
// i. Ako je w ≥1, promjena se prihva ́ca, odnosno xi+1 = xproba.
// ii. Ako je w < 1 generira se sluˇcajni broj r.
// iii. Ako je r ≤w, promjena se prihva ́ca i xi+1 = xproba.
// iv. Ako se promjena ne prihvati onda je xi+1 = xi