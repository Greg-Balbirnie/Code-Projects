#include "collision.h"
#include "graphics/mesh.h"


collision::collision()
{
}


collision::~collision()
{
}

// AABB to AABB Collision
bool collision::AABBtoAABBCollision(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2)
{
	// Get min and max of ball 1 x pos
	float l1 = mesh1->mesh()->aabb().Transform(mesh1->transform()).min_vtx().x();
	float r1 = mesh1->mesh()->aabb().Transform(mesh1->transform()).max_vtx().x();
	// Get min and max of ball 2 x pos
	float l2 = mesh2->mesh()->aabb().Transform(mesh2->transform()).min_vtx().x();
	float r2 = mesh2->mesh()->aabb().Transform(mesh2->transform()).max_vtx().x();
	// Get min and max of ball 1 y pos
	float bo1 = mesh1->mesh()->aabb().Transform(mesh1->transform()).min_vtx().y();
	float t1 = mesh1->mesh()->aabb().Transform(mesh1->transform()).max_vtx().y();
	// Get min and max of ball 2 y pos
	float bo2 = mesh2->mesh()->aabb().Transform(mesh2->transform()).min_vtx().y();
	float t2 = mesh2->mesh()->aabb().Transform(mesh2->transform()).max_vtx().y();
	// Get min and max of ball 1 z pos
	float ba1 = mesh1->mesh()->aabb().Transform(mesh1->transform()).min_vtx().z();
	float f1 = mesh1->mesh()->aabb().Transform(mesh1->transform()).max_vtx().z();
	// Get min and max of ball 2 z pos
	float ba2 = mesh2->mesh()->aabb().Transform(mesh2->transform()).min_vtx().z();
	float f2 = mesh2->mesh()->aabb().Transform(mesh2->transform()).max_vtx().z();

	if (l1 < r2 && r1 > l2 && bo1 < t2 && t1 > bo2 && ba1 < f2 && f1 > ba2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool collision::RayTriangleCollision(gef::Vector4 ray_origin, gef::Vector4 triangle_point[3])
{
	ray_origin -= gef::Vector4(0, 0, 10);
	const float EPSILON = 0.0000001;
	gef::Vector4 edge1, edge2, h, s, q;
	float a, f, u, v;
	gef::Vector4 ray_direction = gef::Vector4(0, 0, 1);	// Straight forward
	edge1 = triangle_point[1] - triangle_point[0];
	edge2 = triangle_point[2] - triangle_point[0];
	h = ray_direction.CrossProduct(edge2);
	a = edge1.DotProduct(h);
	if (a > -EPSILON && a < EPSILON)
	{
		return false;
	}
	f = 1 / a;
	s = ray_origin - triangle_point[0];
	u = f * (s.DotProduct(h));
	if (u < 0.0 || u > 1.0)
	{
		return false;
	}
	q = s.CrossProduct(edge1);
	v = f * ray_direction.DotProduct(q);
	if (v < 0.0 || u + v > 1.0)
	{
		return false;
	}
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * edge2.DotProduct(q);
	if (t > EPSILON) // ray intersection
	{
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}
