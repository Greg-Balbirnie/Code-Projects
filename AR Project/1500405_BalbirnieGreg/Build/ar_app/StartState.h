#pragma once
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <assets\obj_loader.h>
#include <graphics/scene.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/sprite.h>

#include <system/platform.h>
#include <system/application.h>

class ARApp;

class StartState
{
public:
	StartState();
	~StartState();
	void Init(gef::Platform* platform_);
	bool Update(float frame_time, gef::InputManager* input_manager_, ARApp* main_app);
	void Render(gef::SpriteRenderer* sprite_renderer_, gef::Platform* platform_);
private:
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);

	gef::Texture* menu_texture;

	gef::Sprite menu_sprite;
};

