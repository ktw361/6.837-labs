0. It's hard to understand the coordinates setting at the beginning,
    then the order of transformation of matrix is hard.
1. Drawing bones:
    Bones should be drawn by parent, otherwise the bone from parent to current
    node will be affect by rotation.
2. Note in the .obj files, the faces index start from ONE, not ZERO.

SSD -> Deforming the Mesh
3. In vecmath.h, be careful of using (Matrix4f)M = (float)wij * (Matrix4f)N !
    there's no scalar multiply implemented in vecmath.h !
4. Note the difference between Point(x, y, z, 1) vs Vector(x, y, z, 0)
5. Be careful on using MatrixStack.clear()!
