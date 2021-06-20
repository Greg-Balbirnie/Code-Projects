#include "SplashState.h"
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <system/platform.h>
#include <system/application.h>
#include "ar_app.h"

SplashState::SplashState()
{
}


SplashState::~SplashState()
{
}

void SplashState::Init(gef::Platform * platform_)
{
	menu_texture = CreateTextureFromPNG("simpsplash.png", *platform_);

	is_rendered = false;
}

bool SplashState::Update(PlayState& play_state, PrimitiveBuilder* primitive_builder_, gef::Platform* platform_, ARApp* main_app, gef::AudioManager* audio_manager_)
{
	// Check if the screen has been rendered
	if (is_rendered)
	{
		// Initialise game
		play_state.Init(primitive_builder_, platform_, audio_manager_);

		// Set state
		main_app->Set_State(0);

		return true;
	}

	return false;
}

void SplashState::Render(gef::SpriteRenderer* sprite_renderer_, gef::Platform* platform_)
{
	// Render menu sprite
	sprite_renderer_->Begin();
	menu_sprite.set_texture(menu_texture);
	menu_sprite.set_position(gef::Vector4(platform_->width() / 2, platform_->height() / 2, 0.0f, 0.0f));
	menu_sprite.set_height(platform_->height());
	menu_sprite.set_width(platform_->width());
	sprite_renderer_->DrawSprite(menu_sprite);
	sprite_renderer_->End();

	// Screen has been rendered
	is_rendered = true;
}

gef::Texture * SplashState::CreateTextureFromPNG(const char* png_filename, gef::Platform & platform)
{
	gef::PNGLoader png_loader;
	gef::ImageData image_data;
	gef::Texture* texture = NULL;

	// load image data from PNG file 
	png_loader.Load(png_filename, platform, image_data);

	// if the image data is valid, then create a texture from it
	if (image_data.image() != NULL)
		texture = gef::Texture::Create(platform, image_data);

	return texture;
}
