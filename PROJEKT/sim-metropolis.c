// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ran1.c"

#define N 1000   // broj čestica - previsoka gustoća
#define Nw 10    // broj šetača
#define Nk 1000  // broj koraka
#define Nb 11    // broj blokova
#define Nbskip 1 // broj blokova koje preskačemo --- zašto? "zbog stabilizacije?"

float lennardJones(float x1, float x2, float y1, float y2, float z1, float z2)
{
  float E = 1;
  float a;
  float x = (3.4 * pow(10, -10)) / sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2) + pow((z2 - z1), 2)); // x = sigma/r
  x = pow(x, 6);                                                                                    // x = (sigma/r)**6
  if (x <= 1)                                                                                       // ignoriraj parove koji su preblizu
    a = 4 * E * (pow(x, 2) - x);
  else
    a = 0;
  return a;
}

float rxLJforce(float x1, float x2, float y1, float y2, float z1, float z2)
{
  float product;
  float E = 0;                                                                                      // J, za Argon
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
#pragma region // KONSTANTE & VARIJABLE
  int i, j, k, ib;
  long idum = -1234;
  // konstante:
  float m_rho = 6.69 / 1.784 * pow(10, -26);        // masa kroz gustoću - gustoća argona (at STP): 1.784 g/L == kg/metarkubni
  float m_kb = 6.69 / 1.380649 * pow(10, -26 + 23); // masa kroz k_B = 0.004846
  float m_E = 6.69 / 1.65 * pow(10, -26 + 21);      // masa kroz E_lennardjones
  float E_kb = 1.65 / 1.380649 * pow(10, -21 + 23); // E/kB
  float k_B = 1.380649 * pow(10, -23);              // boltzmannova konstanta == kolko?
  float L0 = cbrt(N * m_rho);                       // da bi volumen bio -3x8=24 + 2(100 čestica) = 26 - red veličine realne gustoće
  float E = 1.65 * pow(10, -21);
  // veličine:
  float x[Nw + 1][N + 1]; // broj šetača, broj čestica
  float y[Nw + 1][N + 1]; // broj šetača, broj čestica
  float z[Nw + 1][N + 1]; // broj šetača, broj čestica
  float v[Nw + 1][N + 1]; // brzine čestica
  float L[Nw + 1];        // stranica - ALI ZAŠTO Nw*?? jer svaki walker tj sistem čestica ima svoj L parametar
  float V[Nw + 1];        // volumen - svaki walker tj sistem čestica ima svoj parametar
  float U[Nw + 1];        // unutarnja energija (Upot + Ukin)
  float Uk[Nw + 1];       // kinetička energija
  float Up[Nw + 1];       // potencijalna energija
  float T[Nw + 1];        // temperatura
  float press[Nw + 1];    // tlak
  // promjene
  float x0, y0, z0, v0, vmin = 400, vmax = 500; // prosječna brzina čestice u plinu je oko 450 m/s
  float dL, dLMax = L0 / 100;                   // metara
  float dx, dy, dz, dv;                         // promjene koordinata i brzina
  float dxyzMax = 0.01 * L0, dvMax = 5;         // maksimalne promjene
  // pomoćne varijable
  float delta_V, delta_H, delta_U, delta_W;
  float x_old[N + 1], y_old[N + 1], z_old[N + 1], v_old[N + 1]; // za reversat promjenu koordinata ako se korak odbacuje - mislim da ovo mora bit array po svim česticama?
  float V_old, L_old, U_old, Up_old, Uk_old, T_old, press_old;  // za reversat promjenu u slučaju odbacivanja
  float p, ran;                                                 // vjerojatnost prihvaćanja promjene, random broj pri određivanju odbacivanja/prihvaćanja koraka
  int N_change_volume = 5;
  float V_mean, U_mean, p_mean, T_mean; // vrijednosti u koraku usrednjene po svim šetačima
  int accepted = 0, rejected = 0;
  float ratio;
#pragma endregion

  FILE *data, *koordinate_nakon, *koordinate_prije;
  data = fopen("data.txt", "w");
  koordinate_nakon = fopen("koordinate_nakon.txt", "w");
  koordinate_prije = fopen("koordinate_prije.txt", "w");

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
    Uk[i] = Ukin(v, i);               // * m => SI
    Up[i] = Upot(x, y, z, i);         // * E => SI
    U[i] = Up[i] + m_E * Uk[i];       // * E => SI
    T[i] = 0.6666 * m_kb / N * Uk[i]; // PRERAČUNATO U SI JEDINICE SA STVARNIM VRIJEDNOSTIMA (OSIM ŠTA NISAM SIGURNA JESU LI BRZINE REALNE)
    L[i] = L0;
    V[i] = L0 * L0 * L0;
    press[i] = 1 / V[i] * (N * k_B * T[i] - 1 / (3 * k_B * T[i]) * rfSum(x, y, z, i)); // kaj još s ovim?
    printf("T=%f; p=%f; rfSum=%f; Upot=%f; Ukin=%f; Uuk=%f; E_kb=%f; V=%f; rfSum/V=%f\n", T[i], press[i], rfSum(x, y, z, i), Upot(x, y, z, i), m_E * Uk[i], U[i], E_kb, V[i], 1 / V[i]);
  }

  // for (ib = 1; ib <= Nb; ib++) // po bloku
  // {
  for (i = 1; i <= Nk; i++) // po koracima
  {
    for (j = 1; j <= Nw; j++) // po šetačima - mikrostanja - šetači po 3N dimenzionalnom faznom prostoru
    {
      printf("prije: L/10^8=%f; V/L0^3=%f; T=%f; U=%f; Upot=%f; Ukin=%f\n", L[j] / L0, V[j] / (L0 * L0 * L0), T[j], U[j], Up[j], Uk[j]);
      V_old = V[j];
      L_old = L[j];
      Uk_old = Uk[j];
      Up_old = Up[i];
      U_old = U[j];
      T_old = T[j];
      press_old = press[j];
      for (k = 1; k <= N; k++) // po česticama
      {
        dx = (ran1(&idum) * 2 - 1) * dxyzMax;
        dy = (ran1(&idum) * 2 - 1) * dxyzMax;
        dz = (ran1(&idum) * 2 - 1) * dxyzMax;
        dv = (ran1(&idum) * 2 - 1) * dvMax;
        x_old[k] = x[j][k];
        y_old[k] = y[j][k];
        z_old[k] = z[j][k];
        v_old[k] = v[j][k];
        x[j][k] += dx;
        y[j][k] += dy;
        z[j][k] += dz;
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
        dL = (ran1(&idum) * 2 - 1) * dLMax;
        L[j] = L[j] + dL;
        if (L[j] < 0) // duljina stranice ne može biti negativna
          L[j] = -L[j];
        V[j] = L[j] * L[j] * L[j];
        for (k = 1; k <= N; k++) // po česticama
        {
          x[j][k] = L[j] / L_old * x[j][k]; // skaliranje svih x koordinata faktorom L'/L
          y[j][k] = L[j] / L_old * y[j][k]; // skaliranje svih y koordinata faktorom L'/L
          z[j][k] = L[j] / L_old * z[j][k]; // skaliranje svih z koordinata faktorom L'/L
        }
      }
      // ponovno računanje termodinamičkih vrijednosti
      Uk[j] = Ukin(v, j);         // * m => SI
      Up[j] = Upot(x, y, z, j);   // * E => SI
      U[j] = Up[j] + m_E * Uk[j]; // * E => SI
      T[j] = 0.6666 * m_kb / N * Uk[j];
      press[j] = 1 / V[j] * (N * k_B * T[j] - 1 / (3 * k_B * T[j]) * rfSum(x, y, z, j));
      // delte
      delta_V = V[j] - V_old;
      delta_U = U[j] - U_old;
      delta_H = E * delta_U + press[j] * delta_V - k_B * T[j] * N * log(V[j] / V_old);
      delta_W = 1 / (k_B * T[j]) * delta_H;
      printf("deltaV/L0^3=%f; deltaU=%f; deltaH=%f; deltaW=%f => p=%f\n", delta_V / (L0 * L0 * L0), delta_U, delta_H, delta_W, exp(-delta_W));
      printf("poslije: L/10^8=%f; V/L0^3=%f; T=%f; U=%f; Upot=%f; Ukin=%f\n", L[j] / L0, V[j] / (L0 * L0 * L0), T[j], U[j], Up[j], Uk[j]);
#pragma region // Metropolis algoritam
      if (delta_W > 0)
      {
        p = exp(-delta_W); // vjerojatnost prihvaćanja promjene
        ran = ran1(&idum);
        if (ran < p)
        {
          accepted++;
          ratio = (float)accepted / (float)(accepted + rejected);
          printf("%f\n", ratio);
        }
        else // odbacivanje promjena
        {
          rejected++;
          ratio = (float)accepted / (float)(accepted + rejected);
          printf("%f\n", ratio);
          // resetiranje vrijednosti
          L[i] = L_old;
          V[j] = V_old;
          Uk[j] = Uk_old;
          Up[j] = Up_old;
          U[j] = U_old;
          T[j] = T_old;
          press[j] = press_old;
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
        accepted++;
        ratio = (float)accepted / (float)(accepted + rejected);
        printf("%f\n", ratio);
      }
#pragma endregion
      // kraj petlje šetača
    }
    // // Maksimalnu duljinu koraka podesavamo kako bi prihvacanje bilo oko 50%
    if (ratio > 0.5)
    {
      dxyzMax = dxyzMax * 1.05;
      dvMax = dvMax * 1.05;
      if (i % N_change_volume == 0)
        dLMax = dLMax * 1.05;
    }
    if (ratio < 0.5)
    {
      dxyzMax = dxyzMax * 0.95;
      dvMax = dvMax * 0.95;
      if (i % N_change_volume == 0)
        dLMax = dLMax * 0.95;
    }
// za svaki korak zapisujemo parametre, usrednjene po šetačima
#pragma region // CALCULATING THE MEANS
    // if (ib > Nbskip) // ako je završena stabilizacija
    // {
    V_mean = 0;
    U_mean = 0;
    T_mean = 0;
    p_mean = 0;
    for (j = 1; j <= Nw; j++)
    {
      V_mean += V[j];
      U_mean += U[j];
      T_mean += T[j];
      p_mean += press[j];
    }
    V_mean = V_mean / Nw;
    U_mean = U_mean / Nw;
    T_mean = T_mean / Nw;
    p_mean = p_mean / Nw;
    fprintf(data, "%d\t%f\t%f\t%f\t%f\t%f\n", i, L[1] / L0, V_mean / (L0 * L0 * L0), U_mean, T_mean, p_mean);
    // }
#pragma endregion
  }
  // svaki blok sadrži nekoliko (100, 1000, 5000, ...) koraka
  // nakon svakog bloka pohranjujemo:
  // ▶ indeks bloka ib
  // ▶ ⟨f⟩ - usrednjenu vrijednost veličine f koju smo akumulirali od početka
  // cijele simulacije
  // ▶ ⟨f⟩b - usrednjenu vrijednost veličine f koju smo akumulirali samo
  // unutar jednog bloka
  // }
  // printf("%f\n", ratio);

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
