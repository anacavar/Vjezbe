// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c"

// NpT ansambl -
// The isothermal–isobaric ensemble (constant temperature and constant pressure ensemble)
// is a statistical mechanical ensemble that maintains constant temperature
// T, and constant pressure p, applied. It is also called the
// NpT-ensemble, where the number of particles N, is also kept as a constant.

// ŠTO znači simulirati? koje parametre želimo dobiti? kako kontrolirati za ove parametre? kojim metodama?

// Simulating a Lennard-Jones fluid of Argon using the NPT ensemble involves the following steps:

// Set up the initial configuration of the system. This involves randomly placing particles in a simulation box and assigning initial velocities.

// Choose appropriate values for the temperature, pressure, and volume of the system. These will be the target values for the NPT ensemble simulation.

// Choose appropriate values for the timestep and the number of timesteps to run the simulation for.

// Apply periodic boundary conditions to the simulation box to ensure that particles interact with their images as well as with other particles.

// Use a numerical integration algorithm, such as the Verlet algorithm, to integrate the equations of motion and evolve the system forward in time.

// During each timestep, adjust the box size and particle velocities so that the temperature, pressure, and volume of the system remain at the target values. This is achieved by scaling the velocities of the particles according to the desired temperature and adjusting the box size to maintain the desired pressure.

// Record the properties of the system, such as the temperature, pressure, and energy, at regular intervals during the simulation.

// Analyze the results to determine the thermodynamic properties of the system, such as the radial distribution function, the equation of state, and the diffusion coefficient.

// It is important to note that simulating a Lennard-Jones fluid of Argon using the NPT ensemble is a computationally intensive task, and requires the use of specialized software packages and high-performance computing resources. Popular software packages for molecular dynamics simulations include GROMACS, LAMMPS, and NAMD.

#define N 1000  // broj čestica
#define Nw 1000 // broj šetača == broj čestica??? or not? čemu služe šetači?
#define Nk 1000

int main(void)
{
    // Promatramo ansambl:
    //  Izolirani sustav fiksnog broja čestica 𝑁, pritiska p i temperature T.
    // Glavna razlika u Monte Carlo metodi je postojanje dodatnih koraka koji odgovaraju promjeni volumena
    // ili promjeni broja čestica.

    // ako se ne varam šetaći šeću po faznom prostor, ergo - mikrostanje
    // Za usrednjit po ansamblu
    // Ako u svakom trenutku t imaš Nw točaka faznog prostora

    long idum = -1234;
    float p; // pressure
    float T; // temperature

    float E[N] = {0.};

    FILE *data;
    data = fopen("data.txt", "w");

    int i, j, k;

    int ip; // indeks čestice - šta će mi ovo?
    float Ed, Es;
    float dE;
    float dv, dvMax = 2.0; // 2.0; čega? što je ovo? dinamička varijabla koja se mijenja!!! nakon promjene se mjeri deltaE i provjerava promjena
    // KAJ BUM JA TO MIJENJALA U SVOM NpT ansamblu?
    float vTmp; // v-temporary - čeka prihvaćanje ili odbacivanje promjene

    // // // Glavna razlika u Monte Carlo metodi je postojanje dodatnih koraka koji odgovaraju promjeni volumena

    for (i = 1; i <= Nk; i++) // po koracima
    {
        for (j = 1; j <= Nw; j++)    // po šetačima (ČESTICAMA???) čemu služe šetači??
        {                            // ili obrnuto? čestice pa šetači?
            for (k = 1; k <= N; k++) // po česticama KOjA je razlika meždu čestice i šetači???
            {
                // unutar petlje po šetačima unutar petlje po česticama
                // Nasumično odaberemo česticu i promijenimo joj v za nasumični broj iz [-dvMax ,dvMax]
                ip = (int)(ran1(&idum) * N); // ran1(&idum) = random broj iz[0, 1]
                if (ip == N)
                    ip = N - 1;                       // ip iz {0,1,2,...,N-2,N-1} dv = (2.*ran1(&idum)-1.)*dvMax; vTmp = v + dv;
                dv = (2. * ran1(&idum) - 1.) * dvMax; // OTKUD DOKUD?
                vTmp = v + dv;                        // KAJ ĆE MI OVO? spremaš u temporari varijablu -> slijedi provjera :)

                // // dE = E(vTmp) - E(v); // Izračunamo promjenu energije dE prilikom mijenjanja veličine v
                // // Ako je promjena energije manja ili jednaka 0, sustav daje |dE| demonu i prihvaća se promjena veličine v
                // if (dE <= 0)
                // {
                //     Ed = Ed - dE;
                //     Es = Es + dE;
                //     v = vTmp;
                // }
                // // Ako je promjena veća od 0 i ED>=dE, demon daje energiju sustavu i prihvaća se promjena veličine v
                // // Ako je promjena veće od 0 i ED<dE, ne prihvaća se promjena veličine v pa nema promjene energije
                // if (dE > 0 && Ed >= dE)
                // {
                //     Ed = Ed - dE;
                //     Es = Es + dE;
                //     v = vTmp;
                // }
                if (Ed >= dE)
                {
                    Ed = Ed - dE;
                    Es = Es + dE;
                    v = vTmp;
                }
            }
        }
    }

    return 0;
}