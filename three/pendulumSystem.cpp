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

        Particle ptl(MASS);
        particles.push_back(ptl);

        if (i > 0) {
            Spring spr(LENGTH, STIFFNESS, i-1, i);
            particles[i-1].spr_inds.push_back(springs.size());
            particles[i].spr_inds.push_back(springs.size());
            springs.push_back(spr);
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

        fv.y() += - particles[i].mass * GravityConst;
        fv += - DragConst * getVelocity(i);

        Vector3f sprForce = Vector3f::ZERO;
        for (size_t j = 0; j != particles[i].spr_inds.size(); ++j) {
            Spring &spr = springs[particles[i].spr_inds[j]];
            int ind2 = spr.getOpposite(i);
            Vector3f d = getPosition(i) - getPosition(ind2);
            float d_abs = d.abs();
            sprForce += - spr.k * (d_abs - spr.r) * d.normalized();
        }
        fv += sprForce;
        fv = fv / particles[i].mass;
        
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
    vector<int> spr_inds = particles[visIndex].spr_inds;
    for (size_t i = 0; i != spr_inds.size(); ++i) {
        int end_ind = springs[spr_inds[i]].getOpposite(visIndex);
        Vector3f end_point = getPosition(end_ind);
        drawConnectionLine(start_point, end_point);
    }
}
