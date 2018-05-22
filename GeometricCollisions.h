//
//  GeometricCollisions.h
//
//  A module to check for collisions between simple geometric
//    solids.
//
//  DO NOT MODIFY THIS FILE
//

#ifndef GEOMETRIC_COLLISIONS_H
#define GEOMETRIC_COLLISIONS_H

class Vector3;



//
//  GeometricCollisions
//
//  A namespace to store functions that check for collisions
//    between simple geometric solids.
//

namespace GeometricCollisions
{

//
//  pointVsSphere
//
//  Purpose: A function to test whether the specified point is
//           inside the specified sphere.
//  Parameter(s):
//    <1> point: The position of the point
//    <2> sphere_center: The position of the sphere center
//    <3> sphere_radius: The radius of the sphere
//  Precondition(s):
//    <1> sphere_radius >= 0.0
//  Returns: Whether the point is inside the sphere.  If the
//           point is just on the edge of the sphere, false is
//           returned.
//  Side Effect: N/A
//

	bool pointVsSphere (const Vector3& point,
	                    const Vector3& sphere_center,
	                    double sphere_radius);

//
//  sphereVsSphere
//
//  Purpose: A function to test whether the two specified
//           spheres intersect.
//  Parameter(s):
//    <1> sphere1_center: The position of the first sphere
//                        center
//    <2> sphere1_radius: The radius of the first sphere
//    <3> sphere2_center: The position of the second sphere
//                        center
//    <4> sphere2_radius: The radius of the second sphere
//  Precondition(s):
//    <1> sphere1_radius >= 0.0
//    <2> sphere2_radius >= 0.0
//  Returns: Whether the first and second spheres intersect.  If
//           the spheres are just touching, false is returned.
//  Side Effect: N/A
//

	bool sphereVsSphere (const Vector3& sphere1_center,
	                     double sphere1_radius,
	                     const Vector3& sphere2_center,
	                     double sphere2_radius);

//
//  pointVsCuboid
//
//  Purpose: A function to test whether the specified point is
//           inside the specified axis-aligned retangular
//           cuboid.  A cuboid is like a cube, but it can have a
//           different side length along each axis.
//  Parameter(s):
//    <1> point: The position of the point
//    <2> cuboid_center: The position of the cuboid center
//    <3> cuboid_size: The distance from the cuboid center to
//                     the faces along each axis;
//                     Half the side length along each axis;
//                     Corresponds to the radius of a sphere
//  Precondition(s):
//    <1> cuboid_size.isAllComponentsNonNegative()
//  Returns: Whether the point is inside the cuboid.  If the
//           point is just on the edge of the cuboid, false is
//           returned.
//  Side Effect: N/A
//

	bool pointVsCuboid (const Vector3& point,
	                    const Vector3& cuboid_center,
	                    const Vector3& cuboid_size);

//
//  cuboidVsCuboid
//
//  Purpose: A function to test whether the specified
//           axis-aligned retangular cuboids intersect.  A
//           cuboid is like a cube, but it can have a different
//           side length along each axis.
//  Parameter(s):
//    <1> cuboid1_center: The position of the first cuboid
//                        center
//    <2> cuboid1_size: The distance from the first cuboid
//                      center to the faces along each axis;
//                      Half the side length along each axis;
//                      Corresponds to the radius of a sphere
//    <3> cuboid2_center: The position of the second cuboid
//                        center
//    <4> cuboid2_size: The distance from the second cuboid
//                      center to the faces along each axis
//  Precondition(s):
//    <1> cuboid1_size.isAllComponentsNonNegative()
//    <2> cuboid2_size.isAllComponentsNonNegative()
//  Returns: Whether the 2 cuboids intersect.  If the cuboids
//           are just touching, false is returned.
//  Side Effect: N/A
//

	bool cuboidVsCuboid (const Vector3& cuboid1_center,
	                     const Vector3& cuboid1_size,
	                     const Vector3& cuboid2_center,
	                     const Vector3& cuboid2_size);

//
//  sphereVsCuboid
//
//  Purpose: A function to test whether the specified sphere and
//           cuboid intersect.
//  Parameter(s):
//    <1> sphere_center: The position of the sphere center
//    <2> sphere_radius: The radius of the sphere
//    <3> cuboid_center: The position of the cuboid center
//    <4> cuboid_size: The distance from the cuboid center to
//                     the faces along each axis;
//                     Half the side length along each axis;
//                     Corresponds to the radius of a sphere
//  Precondition(s):
//    <1> sphere_radius >= 0.0
//    <2> cuboid_size.isAllComponentsNonNegative()
//  Returns: Whether the sphere and cuboid intersect.  If they
//           are just touching, false is returned.
//  Side Effect: N/A
//

	bool sphereVsCuboid (const Vector3& sphere_center,
	                     double sphere_radius,
	                     const Vector3& cuboid_center,
	                     const Vector3& cuboid_size);

}  // end of namespace Collisions



#endif
