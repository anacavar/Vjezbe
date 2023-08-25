// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define N 100  // broj čestica
#define Nw 1   // broj šetača
#define Nk 100 // broj koraka
// #define Nb 11    // broj blokova
// #define Nbskip 1 // broj blokova koje preskačemo --- zašto? "zbog stabilizacije?"

float lennardJones(float x1, float x2, float y1, float y2, float z1, float z2)
{
  float E = 1;
  float a;
  float x = (3.4 * pow(10, -10)) / sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2) + pow((z2 - z1), 2)); // x = sigma/r
  x = pow(x, 6);                                                                                    // x = (sigma/r)**6
  if (x <= 1)
    a = 4 * E * (pow(x, 2) - x);
  else
    a = 0;
  return a;
}

float rxLJforce(float x1, float x2, float y1, float y2, float z1, float z2)
{
  float product;
  float E = 1;                                                                                      // J, za Argon
  float x = (3.4 * pow(10, -10)) / sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2) + pow((z2 - z1), 2)); // x = sigma/r
  x = pow(x, 6);                                                                                    // x = (sigma/r)**6
  if (x <= 1)
    product = 24 * E * (2 * pow(x, 2) - x);
  else
    product = 0;
  return product;
}

float rfSum(float x[Nw + 1][N + 1], float y[Nw + 1][N + 1], float z[Nw + 1][N + 1], int i)
{
  float result = 0;
  int a, b;
  for (a = 1; a <= N; a++)
  {
    for (b = a + 1; b <= N; b++)
    {
      result += rxLJforce(x[i][a], x[i][b], y[i][a], y[i][b], z[i][a], z[i][b]);
    }
  }
  return result;
}

float Upot(float x[Nw + 1][N + 1], float y[Nw + 1][N + 1], float z[Nw + 1][N + 1], int i)
{
  float potEtemp = 0;
  int a, b;
  for (a = 1; a <= N; a++)
  {
    for (b = a + 1; b <= N; b++)
    {
      potEtemp += lennardJones(x[i][a], x[i][b], y[i][a], y[i][b], z[i][a], z[i][b]);
    }
  }
  return potEtemp;
}

float Ukin(float v[Nw + 1][N + 1], int i)
{
  float m = 1;
  float U = 0;
  int a;
  for (a = 1; a <= N; a++)
  {
    U += 0.5 * m * v[i][a] * v[i][a];
  }
  return U;
}

int main(void)
{
  int i, j, k;
  int a, b;
  long idum = -1234;
  float m_kb = 6.69 / 1.380649 * pow(10, -26 + 23); // masa kroz k_B = 0.004846
  float m_E = 6.69 / 1.65 * pow(10, -26 + 21);      // masa kroz E_lennardjones
  float k_B = 1.380649 * pow(10, -23);              // boltzmannova konstanta == kolko?
  float L0 = 0.00000001;                            // da bi volumen bio -3x8=24 + 2(100 čestica) = 26 - red veličine realne gustoće
  int ip;                                           // indeks randomly odabrane čestice
  int ib;                                           // indeks bloka
  float x[Nw + 1][N + 1];                           // broj šetača, broj čestica
  float y[Nw + 1][N + 1];                           // broj šetača, broj čestica
  float z[Nw + 1][N + 1];                           // broj šetača, broj čestica
  float v[Nw + 1][N + 1];                           // brzine čestica
  float x0, y0, z0, v0, vmin = 400, vmax = 500;     // prosječna brzina čestice u plinu je oko 450 m/s
  float L[Nw + 1];                                  // stranica - ALI ZAŠTO Nw*?? jer svaki walker tj sistem čestica ima svoj L parametar
  float V[Nw + 1];                                  // volumen - svaki walker tj sistem čestica ima svoj parametar
  float U[Nw + 1];                                  // unutarnja energija (Upot + Ukin)
  float Uk[Nw + 1];                                 // samo kinetička energija
  float T[Nw + 1];                                  // temperatura
  float pressure = 101325;                          // Pascala
  float dL, dLMax = L0 / 10;                        // metara
  float dx, dy, dz, dv;                             // promjene koordinata x i y
  float dxyzMax = 0.1, dvMax = 0.1;
  float V_prije, V_poslije, U_prije, U_poslije, delta_V, delta_H, delta_U, delta_W;
  float ran;                                                    // random broj pri određivanju odbacivanja/prihvaćanja koraka
  float x_old[N + 1], y_old[N + 1], z_old[N + 1], v_old[N + 1]; // za reversat promjenu koordinata ako se korak odbacuje - mislim da ovo mora bit array po svim česticama?
  float V_old, L_old, U_old, Uk_old, T_old, press_old;          // za reversat promjenu u slučaju odbacivanja
  int N_change_volume = 5;
  float V_mean, U_mean, p_mean, T_mean; // vrijednosti u koraku usrednjene po svim šetačima
  float p;                              // vjerojatnost prihvaćanja promjene - but is it?
  int accepted = 0, rejected = 0;
  float ratio;

  FILE *data, *koordinate_nakon, *koordinate_prije;
  data = fopen("data.txt", "w");
  koordinate_nakon = fopen("koordinate_nakon.txt", "w");
  koordinate_prije = fopen("koordinate_prije.txt", "w");

  float postotak_prihvacanja = 0;

  // inicijalizacija čestica
  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      // dobro je da su sve čestice na početku udaljene barem za σ.
      x0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      y0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      z0 = ran1(&idum) * L0; // ran1(&idum) = random broj iz[0, 1]
      v0 = ran1(&idum) * (vmax - vmin) + vmin;
      x[i][j] = x0; // broj šetača, broj čestica
      y[i][j] = y0; // broj šetača, broj čestica
      z[i][j] = z0; // broj šetača, broj čestica
      v[i][j] = v0;
      fprintf(koordinate_prije, "%f\t%f\t%f\n", x0, y0, z0);
    }
    // računamo unutarnju energiju početne konfiguracije
    Uk[i] = Ukin(v, i);                    // NIJE U SI! radi  mase i brzina, treba pomnožit sa kb (  // float m = 6.69 * pow(10, -26); // kg, za Argon)
    U[i] = Upot(x, y, z, i) + m_E * Uk[i]; // valja ovo pomnožit s E pri korištenju, inače jest preračunato...neš ne valja s Upot!!!!!!! zaš ne bi valjalo...?
    T[i] = 0.6666 * m_kb / N * Uk[i];      // PRERAČUNATO U SI JEDINICE SA STVARNIM VRIJEDNOSTIMA (OSIM ŠTA NISAM SIGURNA JESU LI BRZINE REALNE)
    // T[i] = 0.6666 * m_kb / N * Uk[i] / k_B; // PRERAČUNATO U SI JEDINICE SA STVARNIM VRIJEDNOSTIMA (OSIM ŠTA NISAM SIGURNA JESU LI BRZINE REALNE)
    L[i] = L0;
    V[i] = L0 * L0 * L0;
    // pressure je fking kompliciran...
    // press[i] = 1 / V[i] * (N * k_B * T[i] - 1 / (3 * k_B * T[i]) * rfSum(x, y, z, i)); // kaj još s ovim?
    // printf("U=%f\tT=%f\tp=%f, L=%f, V=%f\n", U[i], T[i], press[i], L[i], V[i]);
    printf("m_kb=%f\tT=%f\tUk=%f\tUpot=%f\n", m_kb, T[i], Uk[i], L[i], Upot(x, y, z, i));
  }

  // for (ib = 1; ib <= Nb; ib++) // po bloku
  // {
  for (i = 1; i <= Nk; i++) // po koracima
  {
    for (j = 1; j <= Nw; j++) // po šetačima - mikrostanja - šetači po 3N dimenzionalnom faznom prostoru
    {
      V_prije = V[j];
      // Uk[j] = Ukin(v, j);
      U[j] = Upot(x, y, z, j) + Uk[j];
      U_prije = U[j];
      for (k = 1; k <= N; k++) // po česticama
      {
        dx = (ran1(&idum) * 2 - 1) * dxyzMax;
        dy = (ran1(&idum) * 2 - 1) * dxyzMax;
        dz = (ran1(&idum) * 2 - 1) * dxyzMax;
        dv = (ran1(&idum) * 2 - 1) * dvMax;
        x_old[k] = x[j][k]; // za svaku česticu unutar šetača registriramo zadnju raspodjelu kako bismo mogli reversat stanje ako korak ovog šetača nije prihvaćen
        y_old[k] = y[j][k]; // za svaku česticu unutar šetača registriramo zadnju raspodjelu kako bismo mogli reversat stanje ako korak ovog šetača nije prihvaćen
        z_old[k] = z[j][k]; // za svaku česticu unutar šetača registriramo zadnju raspodjelu kako bismo mogli reversat stanje ako korak ovog šetača nije prihvaćen
        v_old[k] = v[j][k];
        x[j][k] += dx; // broj šetača, broj čestica
        y[j][k] += dy; // broj šetača, broj čestica
        z[j][k] += dz; // broj šetača, broj čestica
        v[j][k] += dv;
        // brzina ne smije biti negativna (jer odražava apsolutnu vrijednost) iako možda ovo opće nije bitno zbog kvadrata?
        if (v[j][k] < 0)
          v[j][k] = -v[j][k];
        // rubni uvjeti
        if (x[j][k] < 0)
          x[j][k] = -x[j][k];
        if (x[j][k] > L[j])
          x[j][k] = L[j] - (x[j][k] - L[j]);
        if (y[j][k] < 0)
          y[j][k] = -y[j][k];
        if (y[j][k] > L[j])
          y[j][k] = L[j] - (y[j][k] - L[j]);
        if (z[j][k] < 0)
          z[j][k] = -z[j][k];
        if (z[j][k] > L[j])
          z[j][k] = L[j] - (z[j][k] - L[j]);
      }

      if (i % N_change_volume == 0) // SVAKI N-ti korak promijeni i volumen...
      {
        V_old = V[j]; // za slučaj odbacivanja promjene tj. koraka
        L_old = L[j]; // za slučaj odbacivanja promjene tj. koraka
        dL = (ran1(&idum) * 2 - 1) * dLMax;
        L[j] = L[j] + dL;
        V[j] = L[j] * L[j] * L[j];
        if (L[j] < 0)
        { // duljina stranice ne može biti negativna
          L[j] = -L[j];
        }
        for (k = 1; k <= N; k++) // po česticama
        {
          x[j][k] = L[j] / (L[j] - dL) * x[j][k]; // skaliranje svih x koordinata faktorom L'/L
          y[j][k] = L[j] / (L[j] - dL) * y[j][k]; // skaliranje svih y koordinata faktorom L'/L
          z[j][k] = L[j] / (L[j] - dL) * z[j][k]; // skaliranje svih z koordinata faktorom L'/L
        }
      }

      // Uk_old = Uk[j];
      U_old = U[j];
      T_old = T[j];
      // press_old = press[j];
      // A GDJE ODBACUJEM OVE KORAKE U i T i press????
      Uk[j] = Ukin(v, j);
      U[j] = Upot(x, y, z, j) + m_E * Uk[j];
      T[j] = 0.6666 * m_kb / N * Uk[j];
      int n = N;
      printf("%f\t%f\t%d\t%f\n", T[j], m_kb, n, Uk[j]);

      // press[j] = 1 / V[j] * (N * k_B * T[j] - 1 / (3 * k_B * T[j]) * rfSum(x, y, z, j));
      V_poslije = V[j];
      U_poslije = U[j];
      delta_V = V_poslije - V_prije;
      delta_U = U_poslije - U_prije;
      // delta_H = delta_U + press[j] * delta_V - k_B * T[j] * N * log(V_poslije / V_prije); // ovdje mijenjaj t??? ili T0 mijenjaj i pressure
      delta_H = delta_U + pressure * delta_V - k_B * T[j] * N * log(V_poslije / V_prije); // ovdje mijenjaj t??? ili T0 mijenjaj i pressure
      // delta_H = delta_U + pressure * delta_V - k_B * T0 * N * log(V_poslije / V_prije); // ovdje mijenjaj t??? ili T0 mijenjaj i pressure
      // delta_W = 1 / (k_B * T0) * delta_H; // beta = 1/(k_B * T) MIJENJAJ T??? ili T0
      delta_W = 1 / (k_B * T[j]) * delta_H; // beta = 1/(k_B * T) MIJENJAJ T??? ili T0
      // printf("deltaH=%f => deltaU=%f; p=%f; deltaV=%f; T=%f\n", delta_H, delta_U, press[j], delta_V, T[j]);
      printf("k_b=%f; T=%f; deltaH=%f => deltaW = %f; ", 1 / k_B, T[j], delta_H, delta_W);
      // Metropolis algoritam
      if (delta_W > 0)
      {
        p = exp(-delta_W); // vjerojatnost prihvaćanja promjene
        printf("p=exp(-deltaW)=%f\n", p);
        // Grubi kriterij za odabir veličine koraka δ je da otprilike jedna trećina do jedna polovica koraka trebaju biti prihvaćeni
        ran = ran1(&idum);
        if (ran < p)
        {
          accepted++;
          ratio = (float)accepted / (float)(accepted + rejected);
          // printf("%f\n", ratio);
        }
        else // odbacivanje promjena
        {
          rejected++;
          Uk[j] = Uk_old;
          U[j] = U_old;
          T[j] = T_old;
          // press[j] = press_old;
          if (i % N_change_volume == 0) // ako je volumen bio promijenjen odbaci promjenu
          {
            L[j] = L_old;
            V[j] = V_old;
          }
          for (k = 1; k <= N; k++) // po česticama
          {
            x[j][k] = x_old[k];
            y[j][k] = y_old[k];
            z[j][k] = z_old[k];
            v[j][k] = v_old[k];
          }
        }
      }
      else
      {
        printf("\n");
        accepted++;
        ratio = (float)accepted / (float)(accepted + rejected);
      }

      // if (i == 305)
      // {
      //   printf("i=%f; V=%f; U=%f; T=%f; p=%f\n", i, V[i], U[i], T[i], press[i]);
      // }

      // kraj šetač koraka
    }

    // printf("ratio = %f\tkorak :%d =? %d: accepted+rejected\n", ratio, i, accepted + rejected);
    // // Maksimalnu duljinu koraka podesavamo kako bi prihvacanje bilo oko 50%
    if (ratio > 0.5)
    {
      // printf("prevelik: ");
      dxyzMax = dxyzMax * 1.05;
      dvMax = dvMax * 1.05;
      if (i % N_change_volume == 0)
        dLMax = dLMax * 1.05;
    }
    if (ratio < 0.5)
    {
      // printf("premali: ");
      dxyzMax = dxyzMax * 0.95;
      dvMax = dvMax * 0.95;
      if (i % N_change_volume == 0)
        dLMax = dLMax * 0.95;
    }

    // printf("%f\t%f\n", ratio, dxMax);
    // Acc_ib = 1. - Acc / ib;
    // if (Acc_ib > 0.5)
    //   dx = dx * 1.05;
    // if (Acc_ib < 0.5)
    //   dx = dx * 0.95;

    // za svaki korak zapisujemo parametre, usrednjene po šetačima
    if (ib > Nbskip) // ako je završena stabilizacija
    {
      V_mean = 0;
      U_mean = 0;
      T_mean = 0;
      p_mean = 0;
      for (j = 1; j <= Nw; j++)
      {
        V_mean += V[j];
        U_mean += U[j];
        T_mean += T[j];
        // p_mean += press[j];
      }
      V_mean = V_mean / Nw;
      U_mean = U_mean / Nw;
      T_mean = T_mean / Nw;
      // p_mean = p_mean / Nw;

      fprintf(data, "%d\t%f\t%f\t%f\n", i, V_mean, U_mean, T_mean);
      // fprintf(data, "%d\t%f\t%f\t%f\t%f\n", i, V_mean, U_mean, T_mean, p_mean);
    }
  }
  // svaki blok sadrži nekoliko(100, 1000, 5000, ...) koraka
  //         nakon svakog bloka pohranjujemo :
  // ▶ indeks bloka ib
  // ▶ ⟨f⟩ -
  //     usrednjenu vrijednost veličine f koju smo akumulirali od početka
  //         cijele simulacije
  // ▶ ⟨f⟩b -
  //     usrednjenu vrijednost veličine f koju smo akumulirali samo
  //         unutar jednog bloka
  // }

  // ratio = (float)accepted / (float)(accepted + rejected);
  // printf("v=%f, %f\n", L[1], ratio); // omjer prihvaćenosti treba biti oko .33-.5 da bi hod po faznom prostoru bio efikasan

  for (i = 1; i <= Nw; i++) // po šetačima
  {
    for (j = 1; j <= N; j++) // po česticama
    {
      fprintf(koordinate_nakon, "%f\t%f\t%f\n", x[i][j], y[i][j], z[i][j]);
    }
  }
  fclose(data);
  fclose(koordinate_nakon);
  fclose(koordinate_prije);
  return 0;
}
