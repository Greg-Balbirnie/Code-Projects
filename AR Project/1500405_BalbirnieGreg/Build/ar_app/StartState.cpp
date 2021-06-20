#include "StartState.h"
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <system/platform.h>
#include <system/application.h>
#include "ar_app.h"

StartState::StartState()
{
}


StartState::~StartState()
{
}

void StartState::Init(gef::Platform* platform_)
{
	menu_texture = CreateTextureFromPNG("Start_Screen.png", *platform_);
}

bool StartState::Update(float frame_time, gef::InputManager* input_manager_, ARApp* main_app)
{
	input_manager_->Update();
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (controller)
	{
		// Check for X to be pressed
		if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
		{
			// Start playing the game
			main_app->Set_State(1);

			return true;
		}
		// Check for square to be pressed
		else if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE)
		{
			// Start playing the game
			main_app->Set_State(3);

			return true;
		}
		else if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
		{
			// Quit the applicaiton
			exit(0);
		}
	}

	return false;
}

void StartState::Render(gef::SpriteRenderer* sprite_renderer_, gef::Platform* platform_)
{
	// Render menu sprite
	sprite_renderer_->Begin();
	menu_sprite.set_texture(menu_texture);
	menu_sprite.set_position(gef::Vector4(platform_->width() / 2, platform_->height() / 2, 0.0f, 0.0f));
	menu_sprite.set_height(platform_->height());
	menu_sprite.set_width(platform_->width());
	sprite_renderer_->DrawSprite(menu_sprite);
	sprite_renderer_->End();
}

gef::Texture * StartState::CreateTextureFromPNG(const char* png_filename, gef::Platform & platform)
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
