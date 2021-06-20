#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/Matrix44.h>
#include <maths/matrix44.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <maths\Matrix44.h>
#include <maths\quaternion.h>

#pragma once
class Marker
{
public:
	Marker();
	~Marker();

	void SetTransform(gef::Matrix44 new_transform);
	gef::Matrix44 GetTransform();

	void SetLocalTransform(gef::Matrix44 origin_transform);
	gef::Matrix44 GetLocalTransform();

	void SetFound(bool found);
	bool GetFound();

private:
	gef::Matrix44 transform;
	gef::Matrix44 local_transform;
	bool is_found;
};

