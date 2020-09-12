#include "TimeStepper.hpp"

// Note the systems are time-invariant, hence we can ignore Time argument

///DONE: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> state = particleSystem->getState(),
        f = particleSystem->evalF(state);
    for (size_t i = 0; i != state.size(); ++i) {
        state[i] = state[i] + stepSize * f[i];
    }
    particleSystem->setState(state);
}

///DONE: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> init = particleSystem->getState(), next = init, newState = init;
    vector<Vector3f> f0 = particleSystem->evalF(init);
    for (size_t i = 0; i != init.size(); ++i) {
        next[i] += stepSize * f0[i];
    }
    vector<Vector3f> f1 = particleSystem->evalF(next);
    for (size_t i = 0; i != newState.size(); ++i) {
        newState[i] += 0.5f * stepSize * (f0[i] + f1[i]);
    }
    particleSystem->setState(newState);
}
