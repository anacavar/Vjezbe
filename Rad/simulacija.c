// Simulate Lennard-Jones fluid of Argon using NPT ansambl

#include <stdio.h>  // standardne ulazno, izlazne funkcije
#include <stdlib.h> // standardne C-funkcije
#include <math.h>
#include "ran1.c"

// NPT ansambl -
// The isothermal–isobaric ensemble (constant temperature and constant pressure ensemble)
// is a statistical mechanical ensemble that maintains constant temperature
// T, and constant pressure P, applied. It is also called the
// NpT-ensemble, where the number of particles N, is also kept as a constant.

// ŠTO znači simulirati? koje parametre želimo dobiti? kako kontrolirati za ove parametre?

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

int main(void)
{

  return 0;
}