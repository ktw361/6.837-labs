Things I learned:
1. Not to confuse * with cross
2. in vecmath: (-1 * Mat * Vec) != -1 * (Mat * Vec), Which is weird!
3. Rotation is vecmath.h is counterclockwise by default.

makeGenCyl()
4. Carefule! pi is 180 degree, pi/2 is 90 degree, pi*2 is 360 degree
5. Not to confuse sweep with profile!
6. makeGenCyl() is easy actually, I overcomplicated it (by making unnecessary
    rotate, F_loc-F_xyz transform, misunderstood sweep coordinates),
   the key is to make sure what is changing & what is not changing,
   e.g., if sweep coor match xyz, then Coefficient C is not chaning.

Artifacts: created in swp/artif{1,2,3}.swp

TODO
3. extra credits, maybe easy?
