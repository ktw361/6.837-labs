#include "vecmath.h"
#include <vector>

#define LOOP_STATE(i, state) \
    for (size_t i = 0; i != state.size(); ++i)

typedef std::vector<Vector3f> stateType;

