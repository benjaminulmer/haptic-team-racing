#include "Magnet.h"

Magnet::Magnet(std::string filename, View view, chai3d::cTransform transform, double strength) : Entity(filename, view, transform), strength(strength) {
	type = Type::MAGNET;
}

// Returns a force that exerts magnetic attraction on the cursor
chai3d::cVector3d Magnet::interact(chai3d::cToolCursor* tool) {

	chai3d::cVector3d toolPos = tool->getLocalTransform() * tool->m_hapticPoint->m_sphereProxy->getLocalPos();

	// Get triangles and vertices of the mesh
	chai3d::cTriangleArrayPtr tris = mesh->getMesh(0)->m_triangles;
	chai3d::cVertexArrayPtr verts = mesh->getMesh(0)->m_vertices;
	int numTris = tris->getNumElements();

	// Need to find closest point on the mesh from the tool
	double minDist = 999.0;
	chai3d::cVector3d point(0.0, 0.0, 0.0);

	for (int i = 0; i < numTris; i++) {

		chai3d::cVector3d v0 = mesh->getLocalTransform() * verts->getLocalPos(tris->getVertexIndex0(i));
		chai3d::cVector3d v1 = mesh->getLocalTransform() * verts->getLocalPos(tris->getVertexIndex1(i));
		chai3d::cVector3d v2 = mesh->getLocalTransform() * verts->getLocalPos(tris->getVertexIndex2(i));

		chai3d::cVector3d proj = chai3d::cProjectPointOnTriangle(toolPos, v0, v1, v2);

		// Store the lowest distance point
		double l = chai3d::cDistance(proj, toolPos);
		if (l < minDist) {
			minDist = l;
			point = proj;
		}
	}

	// Calculate the magnet force
	chai3d::cVector3d dir = toolPos - point;
	double dist = dir.length();
	dir.normalize();


	chai3d::cVector3d force;
	// Prevent instability when close to source
	if (dist < 0.005) {
		force = -dir * dist * 3000.0;
	}
	// Inverse square attraction
	else {
		force = -dir * strength / (dist * dist);
		if (force.length() > 15.0) {
			force.normalize();
			force *= 15.0;
		}
	}

	return force;
}