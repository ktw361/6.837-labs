#pragma once
#include <iostream>
#include <vector>
#include "vecmath.h"
#include <GL/glut.h>

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
