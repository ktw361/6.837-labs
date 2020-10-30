#pragma once

// pendulemSystem.cpp

// Don't change
#define PARTICLE_START_DIST 0.5f
#define GravityConst 9.8f
// End of Don't change

// spring related parameters
#define LENGTH 0.5f
#define STIFFNESS 50.0f
#define MASS 1.5f
#define DragConst 4.0f

#define PENDSYS_NUM_PARTICLES 4


// ClothSystem.h

// Don't change 
#define PARTICLE_INTERVAL 0.2f
// End of Don't change

// Size of cloth
// TODO 3-> 13
#define HEIGHT (3*PARTICLE_INTERVAL)
#define WIDTH  (3*PARTICLE_INTERVAL)
