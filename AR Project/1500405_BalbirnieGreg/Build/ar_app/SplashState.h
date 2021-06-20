#pragma once
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <assets\obj_loader.h>
#include <graphics/scene.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/sprite.h>
#include "PlayState.h"
#include <system/platform.h>
#include <system/application.h>

class ARApp;

class SplashState
{
public:
	SplashState();
	~SplashState();
	void Init(gef::Platform* platform_);
	bool Update(PlayState& play_state, PrimitiveBuilder* primitive_builder_, gef::Platform* platform_, ARApp* main_app, gef::AudioManager* audio_manager_);
	void Render(gef::SpriteRenderer* sprite_renderer_, gef::Platform* platform_);
private:
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);

	gef::Texture* menu_texture;

	gef::Sprite menu_sprite;

	bool is_rendered;
};

