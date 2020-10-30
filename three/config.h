#pragma once

// pendulemSystem.cpp

#define PARTICLE_START_DIST 0.5f
#define GravityConst        9.8f

// spring related parameters
#define LENGTH              0.5f
#define STIFFNESS           50.0f
#define MASS                1.5f
#define VISCOUS             4.0f

#define PENDSYS_NUM_PARTICLES 4


// ClothSystem.h

#define PARTICLE_INTERVAL   0.2f
#define CLO_MASS            0.1f
#define CLO_G               2.0f
#define CLO_LENGTH PARTICLE_INTERVAL
#define CLO_STF_STR         2.0f
#define CLO_STF_SHR         3.0f
#define CLO_STF_FLX         2.0f
#define CLO_VISCOUS         0.5f

// Size of cloth
#define NUM_INTERVALS       10
#define HEIGHT (NUM_INTERVALS*PARTICLE_INTERVAL)
#define WIDTH  (NUM_INTERVALS*PARTICLE_INTERVAL)

#define SWING_Z_LIM         2.0f
#define SWING_SPEED         0.5f

#define WIND_FORCE          0.25f

// Ball for collision
#define BALL_SIZE           1.0f
#define BALL_X              0.5f
#define BALL_Y              -2.75f
#define BALL_Z              0
