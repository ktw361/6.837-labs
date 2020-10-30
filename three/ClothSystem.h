#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <iostream>
#include <vecmath.h>
#include <vector>

#include "particleSystem.h"
#include "common.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(float height, float width);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

private:
    size_t num_rows;
    size_t num_cols;

    SpringParticle particles;

    // Helper functions
    //
    Vector3f getPosition(int ind) { return m_vVecState[2*ind]; }
    Vector3f getVelocity(int ind) { return m_vVecState[2*ind+1]; }

    int indexOf(size_t i, size_t j) {
        if (i < 0 || i >= num_rows || j < 0 || j >= num_cols) {
            std::cerr <<
                "Index Error: ("<<i<<","<<j<<")"<<endl;
            return -1;
        }
        return i * num_cols + j;
    }
};

#endif
