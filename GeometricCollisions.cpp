#include "GeometricCollisions.h"
#include "ObjLibrary/Vector3.h"
#include <cmath>

bool GeometricCollisions::pointVsSphere(const Vector3& point, const Vector3& sphere_center, double sphere_radius)
{
	return (sphere_radius >= sqrt((sphere_center.x - point.x) * (sphere_center.x - point.x) + (sphere_center.y - point.y) * (sphere_center.y - point.y) + (sphere_center.z - point.z) * (sphere_center.z - point.z)));
}

bool GeometricCollisions::sphereVsSphere(const Vector3& sphere1_center, double sphere1_radius,const Vector3& sphere2_center, double sphere2_radius)
{
	return (sphere1_radius + sphere2_radius >= sqrt((sphere1_center.x - sphere2_center.x) * (sphere1_center.x - sphere2_center.x) + (sphere1_center.y - sphere2_center.y) * (sphere1_center.y - sphere2_center.y) + (sphere1_center.z - sphere2_center.z) * (sphere1_center.z - sphere2_center.z)));
}

bool GeometricCollisions::pointVsCuboid(const Vector3& point, const Vector3& cuboid_center, const Vector3& cuboid_size)
{
	if (point.x < cuboid_center.x + cuboid_size.x && point.x > cuboid_center.x - cuboid_size.x)
		if (point.y < cuboid_center.y + cuboid_size.y && point.y > cuboid_center.y - cuboid_size.y)
			if (point.z < cuboid_center.z + cuboid_size.z && point.z > cuboid_center.z - cuboid_size.z)
				return true;
			else
				return false;
		else
			return false;
	else
		return false;
}

bool GeometricCollisions::cuboidVsCuboid(const Vector3& cuboid1_center, const Vector3& cuboid1_size, const Vector3& cuboid2_center, const Vector3& cuboid2_size)
{
	Vector3 cuboid1Max = cuboid1_center + cuboid1_size;
	Vector3 cuboid1Min = cuboid1_center - cuboid1_size;
	Vector3 cuboid2Max = cuboid2_center + cuboid2_size;
	Vector3 cuboid2Min = cuboid2_center - cuboid2_size;
	if (cuboid1Max.x > cuboid2Min.x && cuboid1Min.x < cuboid2Max.x)
		if (cuboid1Max.y > cuboid2Min.y && cuboid1Min.y < cuboid2Max.y)
			if (cuboid1Max.z > cuboid2Min.z && cuboid1Min.z < cuboid2Max.z)
				return true;
			else
				return false;
		else
			return false;
	else
		return false;
}
bool GeometricCollisions::sphereVsCuboid(const Vector3& sphere_center,
	double sphere_radius,
	const Vector3& cuboid_center,
	const Vector3& cuboid_size)
{
	Vector3 closest;
	if (sphere_center.x >= cuboid_center.x + cuboid_size.x)
		closest.x = cuboid_center.x + cuboid_size.x;
	else if (sphere_center.x < cuboid_center.x - cuboid_size.x)
		closest.x = cuboid_center.x - cuboid_size.x;
	else
		closest.x = sphere_center.x;

	if (sphere_center.y >= cuboid_center.y + cuboid_size.y)
		closest.y = cuboid_center.y + cuboid_size.y;
	else if (sphere_center.y < cuboid_center.y - cuboid_size.y)
		closest.y = cuboid_center.y - cuboid_size.y;
	else
		closest.y = sphere_center.y;

	if (sphere_center.z >= cuboid_center.z + cuboid_size.z)
		closest.z = cuboid_center.z + cuboid_size.z;
	else if (sphere_center.z < cuboid_center.z - cuboid_size.z)
		closest.z = cuboid_center.z - cuboid_size.z;
	else
		closest.z = sphere_center.z;

	if (sphere_radius >= sqrt((sphere_center.x - closest.x)*(sphere_center.x - closest.x) + (sphere_center.y - closest.y)*(sphere_center.y - closest.y) + (sphere_center.z - closest.z)*(sphere_center.z - closest.z)))
		return true;
	else
		return false;
}
