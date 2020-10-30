#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <GL/glut.h>

#include "particleSystem.h"
#include "common.h"


class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles, int visIndex = -1);

	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

private:
    int visIndex; // Visible particle index supplied by user.
                     //  -1 indicates none

    SpringParticle particles;

    Vector3f getPosition(int ind) { return m_vVecState[2*ind]; }
    Vector3f getVelocity(int ind) { return m_vVecState[2*ind+1]; }
	
};

#endif
