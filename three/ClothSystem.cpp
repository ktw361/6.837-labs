#include <cstdio>
#include "ClothSystem.h"
#include "config.h"
#include "common.h"

ClothSystem::ClothSystem(float height, float width):
    num_rows(static_cast<size_t>(height/PARTICLE_INTERVAL + 1)),
    num_cols(static_cast<size_t>(width/PARTICLE_INTERVAL + 1))
{
    m_numParticles = num_rows * num_cols;
#ifdef DEBUG
    printf("num_rows:%d, num_cols:%d\n", num_rows, num_cols);
#endif

    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
#ifdef DEBUG
            printf("[%f,%f,%f]\n",PARTICLE_INTERVAL*j,-PARTICLE_INTERVAL*i,0);
#endif

            Vector3f pos(PARTICLE_INTERVAL * j, 0, PARTICLE_INTERVAL * i),
                     vel(0, 0, 0);
            m_vVecState.push_back(pos);
            m_vVecState.push_back(vel);

            particles.particleAdd(MASS);
        }
    }

    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            // Structural springs
            if (j > 0) {
                particles.springAdd(indexOf(i,j-1), indexOf(i,j), CLO_LENGTH, CLO_STIFFNESS);
            }
            if (i > 0) {
                printf("%d, %d\n", indexOf(i-1,j), indexOf(i,j));
                particles.springAdd(indexOf(i-1,j), indexOf(i,j), CLO_LENGTH, CLO_STIFFNESS);
            }

            // Shear springs


            // Flex springs
        }
    }
#ifdef DEBUG
    vector<pair<int, int> > all_pairs = particles.allPairs();
    for (size_t i = 0; i != all_pairs.size(); ++i) {
        printf("pair: %d, %d\n", 
                all_pairs[i].first, all_pairs[i].second);
        getPosition(all_pairs[i].first).print();
        getPosition(all_pairs[i].second).print();
    }
#endif
}


// TODO: implement evalF
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
            fv.y() += - particles.massGet(ind1) * GravityConst;
            fv += - DragConst * getVelocity(ind1);

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
    // Draw all springs
    vector<pair<int, int> > all_pairs = particles.allPairs();
    glBegin(GL_LINES);
    for (size_t i = 0; i != all_pairs.size(); ++i) {
        float *a = getPosition(all_pairs[i].first);
        glVertex3f(a[0], a[1], a[2]);
        float *b = getPosition(all_pairs[i].second);
        glVertex3f(b[0], b[1], b[2]);
    }
    glEnd();
}
