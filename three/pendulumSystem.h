#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <GL/glut.h>

#include "particleSystem.h"

struct Spring {
    float r;
    float k;
    int ind1;
    int ind2;
    Spring(float _r, float _k, int _id1, int _id2) :
        r(_r), k(_k), ind1(_id1), ind2(_id2) {};
    int getOpposite(int ind) {
        if (ind == ind1) return ind2;
        if (ind == ind2) return ind1;
        return -1;
    }
};

/*
 * Each Particle stores list of spring indexes 
 */
struct Particle {
    float mass;
    vector<int> spr_inds;
    Particle(float _mass) : mass(_mass) {};
};

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles, int visIndex = -1);

	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

private:
    int visIndex; // Visible particle index supplied by user.
                     //  -1 indicates none
    vector<Particle> particles;
    vector<Spring> springs;

    Vector3f getPosition(int ind) { return m_vVecState[2*ind]; }
    Vector3f getVelocity(int ind) { return m_vVecState[2*ind+1]; }
	
};

#endif
