#include "ar_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <math.h>
#include "StartState.h"
#include "PlayState.h"
#include "EndState.h"

#include <sony_sample_framework.h>
#include <sony_tracking.h>

ARApp::ARApp(gef::Platform& platform) :
	Application(platform),
	input_manager_(NULL),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	audio_manager_(NULL)
{
}

void ARApp::Init()
{
	input_manager_ = gef::InputManager::Create(platform_);
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	primitive_builder_ = new PrimitiveBuilder(platform_);
	audio_manager_ = gef::AudioManager::Create();

	InitFont();
	SetupLights();

	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);

	// Initialise randomization
	srand(time(NULL));

	// Set up orthographic projection
	ortho_proj.OrthographicFrustumGL(-1, 1, -1, 1, -1, 1);

	//Calculate y() scale variables
	cam_aspect_ratio = (float)CAMERA_WIDTH / (float)CAMERA_HEIGHT;
	display_aspect_ratio = (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT;
	vert_scale_factor = display_aspect_ratio / cam_aspect_ratio;

	// Set sprite to back of frustrum (HARD CODED)
	cam_sprite.set_position(gef::Vector4(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 1));
	
	// Set sprite scale
	cam_sprite.set_width(DISPLAY_WIDTH);
	cam_sprite.set_height(vert_scale_factor * DISPLAY_HEIGHT);

	// Apply texture to sprite
	cam_sprite.set_texture(&tex_vita);

	// Set up proj matrix
	// TODO maybe need to change aspect ratio
	proj_matrix.PerspectiveFovGL(SCE_SMART_IMAGE_FOV, cam_aspect_ratio, 0.01f, 100);

	// Set view matrix to identity
	view_matrix.SetIdentity();

	// Set game state
	current_state = SPLASH;

	state_change = false;

	// initialise current state
	state_init();

	// Set score to zero
	score = 0;

	if (audio_manager_)
	{
		// Set up audio
		audio_manager_->LoadMusic("Music.wav", platform_);

		gef::VolumeInfo music_volume_info;
		music_volume_info.volume = 0.5f;
		audio_manager_->SetMusicVolumeInfo(music_volume_info);

		// Play the audio
		audio_manager_->PlayMusic();
	}
}

void ARApp::CleanUp()
{
	delete primitive_builder_;
	primitive_builder_ = NULL;

	smartRelease();
	sampleRelease();

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;

	if (audio_manager_)
	{
		audio_manager_->UnloadAllSamples();
		audio_manager_->UnloadMusic();
	}

	delete audio_manager_;
	audio_manager_ = NULL;
}

bool ARApp::Update(float frame_time)
{
	switch (current_state)
	{
	case START:
		state_change = start_state.Update(frame_time, input_manager_, this);
		break;
	case PLAY:
		state_change = play_state.Update(frame_time, input_manager_, this, score, audio_manager_);
		break;
	case END:
		state_change = end_state.Update(frame_time, input_manager_, this);
		break;
	case INSTRUCTIONS:
		state_change = instruction_state.Update(frame_time, input_manager_, this);
		break;
	case SPLASH:
		state_change = splash_state.Update(play_state, primitive_builder_, &platform_, this, audio_manager_);
	default:
		break;
	}

	// Check if state has changed
	if (state_change)
	{
		// initialise new state
		state_init();
	}

	return true;
}

void ARApp::Render()
{
	switch (current_state)
	{
	case START:
		start_state.Render(sprite_renderer_, &platform_);
		break;
	case PLAY:
		play_state.Render(proj_matrix, view_matrix, sprite_renderer_, tex_vita, renderer_3d_, cam_aspect_ratio, display_aspect_ratio, vert_scale_factor, cam_sprite, font_, platform_, score);
		break;
	case END:
		end_state.Render(sprite_renderer_, &platform_, font_, score);
		break;
	case INSTRUCTIONS:
		instruction_state.Render(sprite_renderer_, &platform_);
		break;
	case SPLASH:
		splash_state.Render(sprite_renderer_, &platform_);
		break;
	default:
		break;
	}
}

void ARApp::Set_State(int state_value)
{
	// Check value
	switch (state_value)
	{
	case 0:
		current_state = START;
		break;
	case 1:
		// Reset score
		score = 0;
		current_state = PLAY;
		break;
	case 2:
		current_state = END;
		break;
	case 3:
		current_state = INSTRUCTIONS;
		break;
	default:
		break;
	}
}

void ARApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void ARApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void ARApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void ARApp::state_init()
{
	switch (current_state)
	{
	case START:
		start_state.Init(&platform_);
		break;
	case PLAY:
		// Reset score to 0
		score = 0;
		break;
	case END:
		end_state.Init(&platform_);
		break;
	case INSTRUCTIONS:
		instruction_state.Init(&platform_);
		break;
	case SPLASH:
		splash_state.Init(&platform_);
	default:
		break;
	}
}
