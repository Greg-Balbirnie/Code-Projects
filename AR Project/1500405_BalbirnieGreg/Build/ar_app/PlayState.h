#pragma once
#include <system/platform.h>
#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>
#include "primitive_builder.h"
#include "collision.h"
#include "GameObject.h"
#include "Marker.h"
#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <graphics/renderer_3d.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include "Character.h"
#include <vector>
#include <assets\obj_loader.h>
#include <graphics/scene.h>
#include <audio/audio_manager.h>

class ARApp;

class PlayState
{
public:
	PlayState();
	~PlayState();
	void Init(PrimitiveBuilder* primitive_builder_, gef::Platform* platform_, gef::AudioManager* audio_manager_);
	bool Update(float frame_time, gef::InputManager* input_manager_, ARApp* main_app, int& score, gef::AudioManager* audio_manager_);
	void Render(gef::Matrix44 proj_matrix, gef::Matrix44 view_matrix, gef::SpriteRenderer* sprite_renderer_, gef::TextureVita& tex_vita, gef::Renderer3D* renderer_3d_, float cam_aspect_ratio, float display_aspect_ratio, float vert_scale_factor, gef::Sprite cam_sprite, gef::Font* font_, gef::Platform& platform_, int score);
private:
	void RenderOverlay(gef::SpriteRenderer* sprite_renderer_, gef::Platform& platform_, gef::Font* font_, int score);
	void DrawHUD(gef::Font* font_, gef::SpriteRenderer* sprite_renderer_, int score);
	Character SpawnCharacter();
	void RandomiseCharacters();
	gef::Mesh * GetFirstMesh(gef::Scene * scene, gef::Platform* platform_);
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);

	float fps_;

	const float PI = 3.1416;

	// The matrices of the markers
	Marker marker[3];

	// The initial transform of marker 1
	Marker centre;

	// The cubes
	GameObject beam_cube[3];
	GameObject tower[3];

	// The standard matric for a cube
	gef::Matrix44 mesh_matrix;

	// Vectors between markers
	gef::Vector4 direction[3][3];

	float dist;
	float v_dist;	// Difference in Y coord
	float h_dist;	// Difference in X coord
	float angle;	// Angle between then markers

	// The timer for the game
	float timer;
	float time_limit;

	// Create characters
	std::vector<Character> character;

	// The scale of the characters
	gef::Vector4 character_scale;

	// An object to manage collisions
	collision collision_manager;

	// Scenes
	gef::Scene* tower_scene_;
	gef::Scene* bomb_scene_;
	
	// Meshes
	class gef::Mesh* tower_mesh_;
	class gef::Mesh* bomb_mesh_;

	// Textures
	gef::Texture* blue_bomb_tex;
	gef::Texture* red_bomb_tex;
	gef::Texture* tower_tex;
	gef::Texture* lightning_tex;

	// Materials
	gef::Material bomb_mat;
	gef::Material tower_mat;
	gef::Material lightning_mat;

	// Audio ID
	int zap_id;
	
};

