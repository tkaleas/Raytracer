#ifndef STUBS_H
#define STUBS_H

#include <OgreMath.h>

// IMPORTANT NOTE:
// Although these functions are useful for testing, they're not good to use for the
// actual raycasting. In particular, using these functions means you have to invert
// an object's transformation matrix for every ray test you run against it.
// It's much better to cache an object's inverted transformation matrix in the node,
// and then use that to transform the ray, rather than invert it in your raycast.
// Therefore, make these functions wrappers around your actual intersection functions,
// and use those functions in your raytracer.


// Tries to find the intersection of a ray and a sphere.
// P0 is the position from which the ray emanates; V0 is the
// direction of the ray.
// matrix is the transformation matrix of the sphere.
// This function should return the smallest positive t-value of the intersection
// (a point such that P0+t*V0 intersects the sphere), or -1 if there is no
// intersection.
double Test_RaySphereIntersect(
	Vector3 const& P0, Vector3 const& V0,
	Matrix4 const& T);

// Tries to find the intersection of a ray and a triangle.
// This is just like the above function, but it intersects the ray with a
// triangle instead. The parameters p1, p2, and p3 specify the three
// points of the triangle, in object space.
double Test_RayPolyIntersect(
	Vector3 const& P0, Vector3 const& V0,
	Vector3 const& p1, Vector3 const& p2, Vector3 const& p3,
	Matrix4 const& T);

// This is just like Test_RaySphereIntersect, but with a unit cube instead of a
// sphere. A unit cube extends from -0.5 to 0.5 in all axes.
double Test_RayCubeIntersect(
	Vector3 const& P0, Vector3 const& V0,
	Matrix4 const& T);

#endif