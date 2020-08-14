#include "Mesh.h"

using namespace std;

namespace {
    // These are convenience functions which allow us to call OpenGL 
    // methods on Vec3d objects
    inline void glVertex(const Vector3f &a) 
    { glVertex3fv(a); }
    inline void glNormal(const Vector3f &a) 
    { glNormal3fv(a); }
}

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
    std::ifstream istrm(filename, std::ios::in);
    if (!istrm.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    string buf;
    while(getline(istrm, buf)) {
        stringstream ss(buf);
        string s;
        ss >> s;
        if (s == "v") {
            Vector3f v;
            ss >> v[0] >> v[1] >> v[2];
            bindVertices.push_back(v);
        } else if (s == "f") {
            Tuple3u t;
            ss >> t[0] >> t[1] >> t[2];
            faces.push_back(t);
        }
    }

	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".
    glBegin(GL_TRIANGLES);
    for (size_t ind = 0; ind < faces.size(); ++ind) {
        Vector3f v1 = currentVertices[faces[ind][0]-1],
                 v2 = currentVertices[faces[ind][1]-1],
                 v3 = currentVertices[faces[ind][2]-1];
        Vector3f v12 = v2 - v1, v13 = v3 - v1, 
                 v21 = -v12, v23 = v3 - v2,
                 v31 = -v13, v32 = -v23,
                 vn1 = Vector3f::cross(v12, v13).normalized(),
                 vn2 = Vector3f::cross(v23, v21).normalized(),
                 vn3 = Vector3f::cross(v31, v32).normalized();
        ::glNormal(vn1);
        ::glVertex(v1);
        ::glNormal(vn2);
        ::glVertex(v2);
        ::glNormal(vn3);
        ::glVertex(v3);
    }
    glEnd();
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
}
