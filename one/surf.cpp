#include "surf.h"
#include "extra.h"
#ifdef DEBUG
#include <cstdio>
#include <cassert>
#endif
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
#ifdef DEBUG
                printf("profile[%u]: Vz=%f, Tz=%f, Nz=%f\n",
                        i, profile[i].V[2], profile[i].T[2],
                        profile[i].N[2]);
#else
                return false;
#endif
    
        return true;
    }
}

// Build VF for faces, two triangles per quadrilateral
// set closeProfile to true will connect profile's begin and end points.
void buildFace(std::vector<Tup3u> & VF, 
               size_t steps, 
               size_t lenProfile, 
               bool closeProfile) 
{
    unsigned i1, i2, i3, i4;
    for (size_t i = 0; i != steps; ++i) {
        for (size_t j = 0; j != lenProfile - 1; ++j) {
            //
            //  i1 -- i4
            //  |   / |
            //  |  /  |
            //  | /   |
            //  i2 -- i3
            //
            i1 = i * lenProfile + j;
            i2 = i1 + 1;
            i4 = (i != steps-1) ? i1 + lenProfile : j;
            i3 = (i != steps-1) ? i2 + lenProfile : j+1;
            VF.push_back({i1, i2, i4});
            VF.push_back({i4, i2, i3});
        }
        if (closeProfile) {
            i1 = (i * lenProfile) + lenProfile - 1;
            i2 = i * lenProfile;
            i4 = (i != steps-1) ? i1 + lenProfile : lenProfile - 1;
            i3 = (i != steps-1) ? i2 + lenProfile : 0;
            VF.push_back({i1, i2, i4});
            VF.push_back({i4, i2, i3});
        }
    }
#ifdef DEBUG
    for (size_t i = 0; i != VF.size(); ++i)
        printf("F[%lu]: %d, %d, %d\n", i, VF[i][0], VF[i][1], VF[i][2]);
#endif
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    auto & VV = surface.VV;
    auto & VN = surface.VN;
    auto & VF = surface.VF;
    size_t lenProfile = profile.size();
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // Here you should build the surface.  See surf.h for details.

    // Build VV and VN
    for (size_t i = 0; i != steps; ++i) {
        bool isSingular = true;
        float ang = i * 2*M_PI / (float)steps;
        Matrix3f Ry = Matrix3f::rotateY(ang);  // by default counterclock
        Matrix3f Ry_inv_t = Ry.inverse(&isSingular).transposed();
#ifdef DEBUG
        assert(!isSingular);
#endif
        for (size_t j = 0; j != lenProfile; ++j) {
            Vector3f V = Ry * profile[j].V;
            // -1 reverses the norm so that it points OUT of face
            // Note, put -1 outside parenthesis, 
            //  otherwise N_0 will be zero, weird behavior
            Vector3f N = -1 * (Ry_inv_t * profile[j].N);
            VV.push_back(V);
            VN.push_back(N);
        }
    }
#ifdef DEBUG
    for (size_t i = 0; i != VN.size(); ++i)
        printf("V[%lu]: %.2f %.2f %.2f\n",
                i, VV[i][0], VV[i][1], VV[i][2]);
    for (size_t i = 0; i != VN.size(); ++i)
        printf("N[%lu]: %.2f %.2f %.2f\n",
                i, VN[i][0], VN[i][1], VN[i][2]);
#endif

    buildFace(VF, steps, lenProfile, false);

    return surface;
}

Matrix4f makeFrame(const CurvePoint &cp) {
    return Matrix4f(
            Vector4f(cp.N, 0),
            Vector4f(cp.B, 0),
            Vector4f(cp.T, 0),
            Vector4f(cp.V, 1), true);
}

// Originally, we have Point P_abs;
//   P_abs = F_xyz * C
//   P_swp = F_swp * C
//  =>
//   P_swp = F_swp * (F_xyz^-1 * P_abs)
//
// For normal vector, N1 is norm in local frame
//   N1 = M_xyz * C
//   N2 = M_swp * C
//   =>
//   N2 = M_swp * (M_xyz^-1 * N1)
Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;
    auto & VV = surface.VV;
    auto & VN = surface.VN;
    auto & VF = surface.VF;
    size_t lenProfile = profile.size();
    size_t lenSweep = sweep.size();

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // Here you should build the surface.  See surf.h for details.

    Matrix4f F_xyz_inv = Matrix4f::identity().inverse();

    for (size_t i = 0; i != lenSweep; ++i) {
        CurvePoint sp = sweep[i];    // sweep point
        Matrix4f F_swp(makeFrame(sp));
#ifdef DEBUG
        cout << "F_swp: " << endl;
        F_swp.print();
#endif
        for (size_t j = 0; j != lenProfile; ++j) {
            CurvePoint pp = profile[j];  // profile point

            // VV
            Vector4f P_abs_1(pp.V, 1);
            // It's faster to do mat-vec product first
            Vector4f P_swp = F_swp * (F_xyz_inv * P_abs_1);
#ifdef DEBUG
            cout << "P_abs_1: " << endl;
            P_abs_1.print();
            cout << "P_swp: " << endl;
            P_swp.print();
#endif 
            VV.push_back(P_swp.xyz());

            // VN
            Vector3f N = 
                F_swp.getSubmatrix3x3(0,0) *
                (F_xyz_inv.getSubmatrix3x3(0,0) * pp.N);
            N *= -1;  // reverse for pointing out
            VN.push_back(N);
        }
    }
#ifdef DEBUG
    for (size_t i = 0; i != VN.size(); ++i)
        printf("V[%lu]: %.2f %.2f %.2f\n",
                i, VV[i][0], VV[i][1], VV[i][2]);
    for (size_t i = 0; i != VN.size(); ++i)
        printf("N[%lu]: %.2f %.2f %.2f\n",
                i, VN[i][0], VN[i][1], VN[i][2]);
#endif

    buildFace(VF, lenSweep, lenProfile, true);

    return {VV, VN, VF};
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
