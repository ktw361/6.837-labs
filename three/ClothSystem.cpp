#include <cstdio>
#include "ClothSystem.h"
#include "config.h"
#include "common.h"

ClothSystem::ClothSystem(float height, float width):
    num_rows(static_cast<size_t>(height/PARTICLE_INTERVAL + 1)),
    num_cols(static_cast<size_t>(width/PARTICLE_INTERVAL + 1))
{
    m_numParticles = num_rows * num_cols;

    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {

            Vector3f pos(PARTICLE_INTERVAL * j, 0, PARTICLE_INTERVAL * i),
                     vel(0, 0, 0);
            m_vVecState.push_back(pos);
            m_vVecState.push_back(vel);

            particles.particleAdd(CLO_MASS);
        }
    }

    float struc_len = CLO_LENGTH,
          shear_len = 1.4142 * CLO_LENGTH, 
          flex_len = 2 * CLO_LENGTH;
    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            // Structural springs
            if (i > 0)
                particles.springAdd(indexOf(i-1,j), indexOf(i,j), struc_len, CLO_STF_STR);
            if (j > 0)
                particles.springAdd(indexOf(i,j-1), indexOf(i,j), struc_len, CLO_STF_STR);

            // Shear springs
            if (i > 0 && j > 0) {
                particles.springAdd(indexOf(i-1,j-1), indexOf(i,j), shear_len, CLO_STF_SHR);
                particles.springAdd(indexOf(i,j-1), indexOf(i-1,j), shear_len, CLO_STF_SHR);
            }

            // Flex springs
            if (i > 1)
                particles.springAdd(indexOf(i-2,j), indexOf(i,j), flex_len, CLO_STF_FLX);
            if (j > 1)
                particles.springAdd(indexOf(i,j-2), indexOf(i,j), flex_len, CLO_STF_FLX);

        }
    }
}


// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            if (i == 0 && j == 0) {
                f.push_back(Vector3f::ZERO);
                f.push_back(Vector3f::ZERO);
                continue;
            }
            if (i == 0 && j == num_cols-1) {
                f.push_back(Vector3f::ZERO);
                f.push_back(Vector3f::ZERO);
                continue;
            }

            int ind1 = indexOf(i,j);
            Vector3f fx = getVelocity(ind1);
            Vector3f fv = Vector3f::ZERO;

            // Gravity
            fv.y() += - particles.massGet(ind1) * CLO_G;
            fv += - CLO_VISCOUS * getVelocity(ind1);

            // Spring forces
            Vector3f sprForce = Vector3f::ZERO;
            vector<int> const &connects = particles.connects(ind1);
            for (size_t k = 0; k != connects.size(); ++k) {
                int ind2 = connects[k];
                sprForce += particles.force(
                    ind1, ind2, getPosition(ind1), getPosition(ind2));
            }
            fv += sprForce;
            fv = fv / particles.massGet(ind1);

            f.push_back(fx);
            f.push_back(fv);
        }
    }
    return f;
}

// render the system (ie draw the particles)
void ClothSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos; //  position of particle i. YOUR CODE HERE
        pos = getPosition(i);
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.050f,10.0f,10.0f);
		glPopMatrix();
	}
    // Draw structural springs
    glBegin(GL_LINES);
    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            Vector3f pos = getPosition(indexOf(i,j));
            if (i > 0) {
                Vector3f end1 = getPosition(indexOf(i-1,j));
                glVertex3f(pos[0], pos[1], pos[2]);
                glVertex3f(end1[0], end1[1], end1[2]);
            }
            if (j > 0) {
                Vector3f end2 = getPosition(indexOf(i,j-1));
                glVertex3f(pos[0], pos[1], pos[2]);
                glVertex3f(end2[0], end2[1], end2[2]);
            }
        }
    }
    glEnd();
}
