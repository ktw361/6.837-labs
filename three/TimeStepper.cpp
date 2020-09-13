#include "TimeStepper.hpp"
#include "common.h"

// Note the systems are time-invariant, hence we can ignore Time argument

// Helper functions
void stateAdd(stateType &state, stateType &delta, float mul) {
    LOOP_STATE(i, state) {
        state[i] += mul * delta[i];
    }
}

///DONE: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    stateType state = particleSystem->getState(),
        f = particleSystem->evalF(state);
    stateAdd(state, f, stepSize);
    particleSystem->setState(state);
}

///DONE: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    stateType state = particleSystem->getState(), next = state;
    stateType f0 = particleSystem->evalF(state);
    stateAdd(next, f0, stepSize);
    stateType f1 = particleSystem->evalF(next);
    LOOP_STATE(i, next) {
        state[i] += 0.5f * stepSize * (f0[i] + f1[i]);
    }
    particleSystem->setState(state);
}

void MyRK4::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    stateType state, x1=state, x2=state, x3=state,
              k1, k2, k3, k4;
    state = particleSystem->getState();
    k1 = particleSystem->evalF(state);
    stateAdd(x1, k1, stepSize * 0.5f);
    k2 = particleSystem->evalF(x1);
    stateAdd(x2, k2, stepSize * 0.5f);
    k3 = particleSystem->evalF(x2);
    stateAdd(x3, k3, stepSize);
    k4 = particleSystem->evalF(x3);
    LOOP_STATE(i, state) {
        state[i] += stepSize * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]) / 6.0f;
    }
}
