#pragma once

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>

namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;
}

class collision
{
public:
	collision();
	~collision();
	bool AABBtoAABBCollision(gef::MeshInstance*, gef::MeshInstance*);
	bool RayTriangleCollision(gef::Vector4 ray_origin, gef::Vector4 trangle_point[3]);
};

