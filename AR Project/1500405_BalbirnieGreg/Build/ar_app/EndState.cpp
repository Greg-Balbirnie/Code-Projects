#include "EndState.h"
#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <system/platform.h>
#include <system/application.h>
#include "ar_app.h"

EndState::EndState()
{
}


EndState::~EndState()
{
}


void EndState::Init(gef::Platform* platform_)
{
	screen_texture = CreateTextureFromPNG("End_Screen.png", *platform_);
}

bool EndState::Update(float frame_time, gef::InputManager* input_manager_, ARApp* main_app)
{
	input_manager_->Update();
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (controller)
	{
		// Check for circle to be pressed
		if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
		{
			// Go to menu
			main_app->Set_State(0);

			return true;
		}
	}

	return false;
}

void EndState::Render(gef::SpriteRenderer* sprite_renderer_, gef::Platform* platform_, gef::Font* font_, int score)
{
	// Render menu sprite
	sprite_renderer_->Begin();
	screen_sprite.set_texture(screen_texture);
	screen_sprite.set_position(gef::Vector4(platform_->width() / 2, platform_->height() / 2, 0.0f, 0.0f));
	screen_sprite.set_height(platform_->height());
	screen_sprite.set_width(platform_->width());

	font_->RenderText(sprite_renderer_, gef::Vector4(480.0f, 272.0f, -0.9f), 3.0f, 0xffffffff, gef::TJ_CENTRE, "You scored %i", score);

	sprite_renderer_->DrawSprite(screen_sprite);
	sprite_renderer_->End();
}

gef::Texture * EndState::CreateTextureFromPNG(const char * png_filename, gef::Platform & platform)
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
