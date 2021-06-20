#include "Marker.h"



Marker::Marker()
{
}


Marker::~Marker()
{
}

void Marker::SetTransform(gef::Matrix44 new_transform)
{
	transform = new_transform;
}

gef::Matrix44 Marker::GetTransform()
{
	return transform;
}

void Marker::SetLocalTransform(gef::Matrix44 origin_transform)
{
	// get the inverse transform of this marker
	gef::Matrix44 inv_origin_transform;
	inv_origin_transform.AffineInverse(origin_transform);

	// calculate the local transform for marker 2
	local_transform = transform * inv_origin_transform;
}

gef::Matrix44 Marker::GetLocalTransform()
{
	return local_transform;
}

void Marker::SetFound(bool found)
{
	is_found = found;
}

bool Marker::GetFound()
{
	return is_found;
}
