#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

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
#include "StartState.h"
#include "PlayState.h"
#include "EndState.h"
#include "InstructionState.h"
#include "SplashState.h"
#include <audio/audio_manager.h>

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>

// FRAMEWORK FORWARD DECLARATIONS
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

class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
	void Set_State(int state_value);
private:
	void InitFont();
	void CleanUpFont();

	void SetupLights();

	void state_init();

	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;

	class gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;

	// The orthographic projection
	gef::Matrix44 ortho_proj;

	// Aspect ratio variables
	float cam_aspect_ratio;
	float display_aspect_ratio;
	float vert_scale_factor;

	// Camera image sprite
	gef::Sprite cam_sprite;

	// Vita tex
	gef::TextureVita tex_vita;

	// Transform matrix
	gef::Matrix44 proj_matrix;
	gef::Matrix44 view_matrix;

	// The scale of the markers
	gef::Vector4 marker_scale;

	// Game state machine
	enum Game_State
	{
		START,
		PLAY,
		END,
		INSTRUCTIONS,
		SPLASH
	};

	Game_State current_state;

	// State Classes
	StartState start_state;
	PlayState play_state;
	EndState end_state;
	InstructionState instruction_state;
	SplashState splash_state;
	bool state_change;

	// The player's score
	int score;
};

#endif // _RENDER_TARGET_APP_H