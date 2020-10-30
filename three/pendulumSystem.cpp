#include "pendulumSystem.h"
#include "config.h"
#include "common.h"

// e.g, m = 4: 0, 0.5, 1.0, 1.5
PendulumSystem::PendulumSystem(int numParticles, int visIndex):
    ParticleSystem(numParticles), visIndex(visIndex)
{
	m_numParticles = numParticles;

	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		
		// for this system, we care about the position and the velocity
        Vector3f pos(PARTICLE_START_DIST * i, 0, 0), vel(0, 0, 0);
        m_vVecState.push_back(pos);
        m_vVecState.push_back(vel);

        particles.particleAdd(MASS);

        if (i > 0) {
            particles.springAdd(i-1, i, LENGTH, STIFFNESS);
        }
	}
}

// DONE: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	// YOUR CODE HERE
    // The first particle does not move.
    f.push_back(Vector3f::ZERO);
    f.push_back(Vector3f::ZERO);

    for (int i = 1; i < m_numParticles; ++i) {
        Vector3f fx = getVelocity(i);
        Vector3f fv = Vector3f::ZERO;

        fv.y() += - particles.massGet(i) * GravityConst;
        fv += - DragConst * getVelocity(i);

        Vector3f sprForce = Vector3f::ZERO;
        vector<int> const &connects = particles.connects(i);
        for (size_t j = 0; j != connects.size(); ++j) {
            int ind2 = connects[j];
            sprForce += particles.force(i, ind2, getPosition(i), getPosition(ind2));
        }
        fv += sprForce;
        fv = fv / particles.massGet(i);
        
        f.push_back(fx);
        f.push_back(fv);
    }

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos; //  position of particle i. YOUR CODE HERE
        pos = getPosition(i);
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
    // Draw springs 
    if (visIndex == -1) return;
    Vector3f start_point = getPosition(visIndex);
    vector<int> const &connects = particles.connects(visIndex);
    for (size_t i = 0; i != connects.size(); ++i) {
        drawSpring(start_point, getPosition(connects[i]));
    }
}
