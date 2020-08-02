#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#ifdef DEBUG
#include <cassert>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}


// Common function, to be called in evalBezier and evalBspline
//
// For Bezier, p_step = 3; For Bspline p_step = 1
//
// For every 4 points:
//  P(t) = Geomery G x Spline Basis B x Power Basis T(t)
//
//  then for every i in [step_begin, step_end):
//    t = i / num_steps;
//    V_i = P(i)
//    T_i = P'(i).normalized()
//    N_i = (B_{i-1} x T_i).normalized()
//    B_i = (T_i x N_i).normalized()
Curve evalCommon( const vector< Vector3f >& P, 
                  Matrix4f & B,
                  unsigned p_step,
                  unsigned step_bgn,
                  unsigned step_end,
                  unsigned num_steps ) 
{
    Curve result;
    for (size_t i = 0; i < P.size() - 3; i += p_step) {
        Matrix4f G(Vector4f(P[i], 0), 
                Vector4f(P[i+1], 0),
                Vector4f(P[i+2], 0),
                Vector4f(P[i+3], 0), true);

        Vector3f Bino(0, 0, 1);  // B_0
        for (size_t t = step_bgn; t != step_end; ++t) {
            float _t = (float)t / (float)num_steps;
            Vector4f T(1, _t, _t*_t, _t*_t*_t);
            Vector4f dT(0, 1, 2*_t, 3*_t*_t);

            Vector3f Vert = (G * B * T).xyz();
            Vector3f Tang = (G * B * dT).xyz();
            Tang = Tang.normalized();
            // Test B_0, until parallel to T1: add 1 unit to a axis
            if (t == 0) {
                for (   int l = 0;
                        approx(Vector3f::cross(Bino, Tang), Vector3f::ZERO);
                        ++i
                    ) {
                    Bino[l%3] += 1;
                    Bino = Bino.normalized();
                }
#ifdef DEBUG
            } else {
                assert(!approx(Vector3f::cross(Bino, Tang), Vector3f::ZERO));
#endif 
            }
            Vector3f Norm = Vector3f::cross(Bino, Tang).normalized();
            Vector3f Bino = Vector3f::cross(Tang, Norm).normalized();
            result.push_back( {Vert, Tang, Norm, Bino} );
#ifdef DEBUG
            printf("curve[%lu]:\n", i*(step_end-step_bgn) + t);
            printf("\tV = %.2f %.2f %.2f\n", Vert[0], Vert[1], Vert[2]);
            printf("\tT = %.2f %.2f %.2f\n", Tang[0], Tang[1], Tang[2]);
            printf("\tN = %.2f %.2f %.2f\n", Norm[0], Norm[1], Norm[2]);
            printf("\tB = %.2f %.2f %.2f\n", Bino[0], Bino[1], Bino[2]);
#endif
        }
    }
    return result;
}

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // My code here:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    Curve result;

    Matrix4f B(1, -3, 3, -1,
               0,  3,-6,  3,
               0,  0, 3, -3,
               0,  0, 0,  1);
    return evalCommon(P, B, 3, 0, steps, steps);
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // My code here:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.
    Matrix4f B(1, -3,  3, -1,
               4,  0, -6,  3,
               1,  3,  3, -3,
               0,  0,  0,  1);
    // The problem with Matrix4f::uniformScaling is that is doesn't set
    //   B[3][3] = B[3][3] / 6
    B /= 6.0f;

    return evalCommon(P, B, 1, 0, steps, steps);
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

