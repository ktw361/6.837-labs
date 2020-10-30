#pragma once
#include <iostream>
#include <vector>
#include "vecmath.h"
#include <GL/glut.h>

#include <map>
#include <utility>  // std::pair


#define LOOP_STATE(i, state) \
    for (size_t i = 0; i != state.size(); ++i)

typedef std::vector<Vector3f> stateType;

inline void display_modelview(GLfloat mat[16])
{
    // Column-based
    cout << "ModelView: " << endl;
    for (int i = 0; i != 4; ++i) {
        for (int j = 0; j != 4; ++j)
            cout << mat[j*4+i] << " ";
        cout << endl;
    }
}

inline void drawConnectionLine(Vector3f const &start, Vector3f const &end) {
    // Change of Basis.
    // 
    // new Z-axis points from start to end
    Vector3f offset = end - start;
    float dis = offset.abs();
    Matrix4f new_frame = Matrix4f::identity(), trans_start = new_frame;
    trans_start.setCol(3, Vector4f(start, 1.0f)); 
    Vector3f rnd(0, 0, 1.0f),
             z = offset.normalized(),
             y = Vector3f::cross(z, rnd).normalized(),
             x = Vector3f::cross(y, z).normalized();
    new_frame.setSubmatrix3x3(0, 0, Matrix3f(x, y, z, true));
    Matrix4f scale = Matrix4f::scaling(0.03f, 0.03f, dis),
             trans = Matrix4f::translation(0, 0, 0.5f);
    glPushMatrix();
    glMultMatrixf(trans_start);
    glMultMatrixf(new_frame);
    glMultMatrixf(scale);
    glMultMatrixf(trans);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Interface of SpringParticle:
//  1. Add Particle;
//  2. Add Spring that connects particle (i,j);
//  3. Get mass of a particle;
//  4. Get a list of 'j' that connect to 'i';
//  5. Compute force on 'i' position, given 'j' position.
class SpringParticle {

private:
    struct Spring {
        float r;
        float k;
        int ind1;
        int ind2;
        Spring(float _r, float _k, int _id1, int _id2) :
            r(_r), k(_k), ind1(_id1), ind2(_id2) {};
        int getOpposite(int ind) const {
            if (ind == ind1) return ind2;
            if (ind == ind2) return ind1;
            return -1;
        }
    };

    // Each Particle stores list of spring indexes 
    struct Particle {
        float mass;
        vector<int> spr_inds;
        Particle(float _mass) : mass(_mass) {};
    };

public:
    // Add Particle with mass
    //
    // Returns:
    //  index of added particle
    int
    particleAdd(float mass) {
        particles.push_back(Particle(mass));
        return particles.size() - 1;
    }
    
    // Add Spring that connects particle (i,j)
    int 
    springAdd(int i, int j, float length, float stiffness) {
        // At most one spring for each pair
        if (pair_map.count(std::make_pair(i,j)) != 0) {
            std::cerr <<
                "Redundant pair (" << i << "," << j << ")"
                << endl;
            return -1;
        }
        Spring spr(length, stiffness, i, j);
        int spr_ind = springs.size();
        particles[i].spr_inds.push_back(spr_ind);
        particles[j].spr_inds.push_back(spr_ind);
        pair_map[std::make_pair(i, j)] = spr_ind;
        pair_map[std::make_pair(j, i)] = spr_ind;
        springs.push_back(spr);
        return spr_ind;
    }

    // Get mass of a particle
    float 
    massGet(int i) {
        return particles[i].mass;
    }

    //  Get a list of 'j' that connect to 'i'
    vector<int> 
    connects(int i) const {
        vector<int> opposites;
        for (size_t j = 0; j != particles[i].spr_inds.size(); ++j) {
            Spring const &spr = springs[particles[i].spr_inds[j]];
            opposites.push_back(spr.getOpposite(i));
        }
        return opposites;
    }

    //  Compute force on 'i' position, given 'j' position.
    Vector3f 
    force(int i, int j, Vector3f i_pos, Vector3f j_pos) const {
        if (pair_map.count(std::make_pair(i,j)) == 0) {
            std::cerr <<
                "Spring not found for (" << i << "," << j << ")"
                << endl;
            return Vector3f::ZERO;
        }
        Vector3f d = i_pos - j_pos;
        float d_abs = d.abs();
        int spr_ind = pair_map.find(std::make_pair(i,j))->second;
        Spring const &spr = springs[spr_ind];
        return - spr.k * (d_abs - spr.r) * d.normalized();
    }

private:
    vector<Particle> particles;
    vector<Spring> springs;
    std::map<std::pair<int, int>, int> pair_map;
};
