0. It's hard to understand the coordinates setting at the beginning,
    then the order of transformation of matrix is hard.
1. Drawing bones:
    Bones should be drawn by parent, otherwise the bone from parent to current
    node will be affect by rotation.
2. Note in the .obj files, the faces index start from ONE, not ZERO.
