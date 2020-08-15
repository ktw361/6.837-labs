#include "SkeletalModel.h"
#include <functional>
#ifdef DEBUG
#include <cassert>
#endif
#include <vecmath.h>

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
    this->cameraMatrix = cameraMatrix;

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
        m_matrixStack.push(this->cameraMatrix);

		// Clear out any weird matrix we may have been using 
        // for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
    std::ifstream istrm(filename, std::ios::in);
    if (!istrm.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    string buf;
    while(getline(istrm, buf)) {
        stringstream ss(buf);
        float f1, f2, f3;
        int index;
        ss >> f1 >> f2 >> f3 >> index;
        Joint *joint = new Joint;
        joint->transform = Matrix4f::translation(f1, f2, f3);
        // Assuming joints occurs in accending order!
        if (index == -1) {
            m_rootJoint = joint;
        } else {
#ifdef DEBUG
            Joint *parent = m_joints.at(index);
#else
            Joint *parent = m_joints[index];
#endif
            parent->children.push_back(joint);
        }
        m_joints.push_back(joint);
        ori_trans.push_back(joint->transform);
    }
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function 
    // to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
    std::function<void(Joint*)> bfs = [&, this](Joint *jt) -> void {
        m_matrixStack.push(jt->transform);
        glLoadMatrixf(m_matrixStack.top());
        glutSolidSphere(0.025f, 12, 12);
        for (auto it = jt->children.begin(); it != jt->children.end(); ++it) 
            bfs(*it);
        m_matrixStack.pop();
    };
    // We have cameraMatrix here, hence we must not clear()
    m_matrixStack.push(cameraMatrix);
    bfs(m_rootJoint);
    m_matrixStack.pop();
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. 
    // You will need to add a recursive helper function to traverse the joint hierarchy.
    std::function<void(Joint*)> bfs = [&, this](Joint *jt) -> void {
        Vector3f offset = jt->transform.getCol(3).xyz(); // from parent to jt
        float dis = offset.abs();
        // There're two ways, one is by rotation, another one is by Matrix of Basis,
        // we use the Change of Basis.
        // 
        // We're still at parent's transform frame,
        // new Z-axis points from parent to children
        auto new_frame = Matrix4f::identity();
        Vector3f rnd(0, 0, 1.0f),
                 z = offset.normalized(),
                 y = Vector3f::cross(z, rnd).normalized(),
                 x = Vector3f::cross(y, z).normalized();
        new_frame.setSubmatrix3x3(0, 0, Matrix3f(x, y, z, true));
        auto scale = Matrix4f::scaling(0.05f, 0.05f, dis),
             trans = Matrix4f::translation(0, 0, 0.5f);
        m_matrixStack.push(new_frame);
        m_matrixStack.push(scale);
        m_matrixStack.push(trans);
        glLoadMatrixf(m_matrixStack.top());
        glutSolidCube(1.0f);
        m_matrixStack.pop();
        m_matrixStack.pop();
        m_matrixStack.pop();
        //
        // The `rotation` way, somewhat different result
        //
        // Vector3f origin(0, 0, 1.0f);
        // auto dir = Vector3f::cross(origin, offset);
        // float radians = acos(Vector3f::dot(origin.normalized(), offset.normalized()));
        // m_matrixStack.push(Matrix4f::rotation(dir, radians));
        // m_matrixStack.push(Matrix4f::scaling(0.05f, 0.05f, dis));
        // m_matrixStack.push(Matrix4f::translation(0, 0, 0.5f));
        // glLoadMatrixf(m_matrixStack.top());
        // glutSolidCube(1.0f);
        // m_matrixStack.pop();
        // m_matrixStack.pop();
        // m_matrixStack.pop();

        m_matrixStack.push(jt->transform);
        for (auto it = jt->children.begin(); it != jt->children.end(); ++it)
            bfs(*it);
        m_matrixStack.pop();
    };
    // We have cameraMatrix here, hence we must not clear()
    m_matrixStack.push(cameraMatrix);
    m_matrixStack.push(m_rootJoint->transform);
    for (auto it = m_rootJoint->children.begin();
            it != m_rootJoint->children.end();
            ++it) bfs(*it);
    m_matrixStack.pop();
    m_matrixStack.pop();
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix 
    // based on the passed in Euler angles.
    auto Rx = Matrix4f::rotateX(rX),
         Ry = Matrix4f::rotateY(rY),
         Rz = Matrix4f::rotateZ(rZ);
    m_joints[jointIndex]->transform = 
        ori_trans[jointIndex] * Rx * Ry * Rz;
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
    std::function<void(Joint*)> bfs = [&, this](Joint *jt) -> void {
        m_matrixStack.push(jt->transform);
        // bindWorldToJointTransform = B^-1
        jt->bindWorldToJointTransform = m_matrixStack.top().inverse();
        for (auto it = jt->children.begin(); it != jt->children.end(); ++it) 
            bfs(*it);
        m_matrixStack.pop();
    };
    m_matrixStack.clear();
    bfs(m_rootJoint);
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform
    // (*fix*: currentJointToWorldTransform).
	// You will need to add a recursive helper function to traverse the joint hierarchy.
    std::function<void(Joint*)> bfs = [&, this](Joint *jt) -> void {
        m_matrixStack.push(jt->transform);
        jt->currentJointToWorldTransform = m_matrixStack.top();
        for (auto it = jt->children.begin(); it != jt->children.end(); ++it) 
            bfs(*it);
        m_matrixStack.pop();
    };
    m_matrixStack.clear();
    bfs(m_rootJoint);
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.

    // Some notes:
    //  1. Matrix4f does not support `+`, also no scalar multiplication.
    //  2. Distinguish between point( [x,y,z,1] ) and vector ( [x,y,z,0] )
    const size_t numVertices = m_mesh.bindVertices.size();
    const size_t numJoints = m_joints.size();
    for (size_t i = 0; i != numVertices; ++i) {
        Vector4f p_bind(m_mesh.bindVertices[i], 1.0f); // 1.0f indicats a POINT
        // p_i = \sum_j w_ij T_j * B^-1_j * p_i
        Vector4f b1, b2, b3, b4; 
        for (size_t j = 0; j != numJoints; ++j) {
            float wij = m_mesh.attachments[i][j];
            Matrix4f bind2Joint = m_joints[j]->bindWorldToJointTransform,
                     joint2World = m_joints[j]->currentJointToWorldTransform;
            Matrix4f M = joint2World * bind2Joint;
            b1 = b1 + wij * M.getCol(0);
            b2 = b2 + wij * M.getCol(1);
            b3 = b3 + wij * M.getCol(2);
            b4 = b4 + wij * M.getCol(3);
        }
        Vector4f deform = Matrix4f(b1, b2, b3, b4) * p_bind; 
        m_mesh.currentVertices[i] = deform.xyz();
    }
}

