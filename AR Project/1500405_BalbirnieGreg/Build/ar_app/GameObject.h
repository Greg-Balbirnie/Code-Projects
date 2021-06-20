#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/Matrix44.h>
#include <maths/matrix44.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include "primitive_builder.h"
#include <maths\Matrix44.h>
#include <maths\quaternion.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class InputManager;
	class Scene;
}

#pragma once
class GameObject : public gef::MeshInstance
{
public:
	GameObject();
	~GameObject();

	gef::Vector4 Get_Position();
	void Set_Position(gef::Vector4 new_position);

	void Set_Rotation(float rot_x, float rot_y, float rot_z);

	gef::Vector4 Get_Scale();
	void Set_Scale(gef::Vector4 new_scale);

	void set_transform(gef::Matrix44 new_transform);
private:
	void Build_Transform();

	float rotation_x;
	float rotation_y;
	float rotation_z;
};

