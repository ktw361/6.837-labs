
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem()
{
    m_vVecState.push_back(Vector3f(1.0f, 0, 0));
}

// DONE: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	// YOUR CODE HERE
    for (size_t i = 0; i != state.size(); ++i) {
        Vector3f dx = Vector3f();
        dx.x() = - state[i].y();
        dx.y() = state[i].x();
        dx.z() = 0.0f;
        f.push_back(dx);
    }

	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{
    Vector3f pos;//YOUR PARTICLE POSITION
    pos = m_vVecState[0];

    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2] );
    glutSolidSphere(0.075f,10.0f,10.0f);
    glPopMatrix();
}
