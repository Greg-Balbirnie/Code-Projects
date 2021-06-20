#include "PlayState.h"
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
#include "ar_app.h"
#include <assets/png_loader.h>
#include <graphics/image_data.h>

PlayState::PlayState()
{
}

PlayState::~PlayState()
{
}

void PlayState::Init(PrimitiveBuilder* primitive_builder_, gef::Platform* platform_, gef::AudioManager* audio_manager_)
{
	// create a new scene object and read in the data from the file
	// no meshes or materials are created yet
	// we're not making any assumptions about what the data may be loaded in for
	tower_scene_ = new gef::Scene();
	bomb_scene_ = new gef::Scene();

	tower_scene_->ReadSceneFromFile(*platform_, "tower.scn");
	bomb_scene_->ReadSceneFromFile(*platform_, "bomb.scn");

	// we do want to render the data stored in the scene file so lets create the materials from the material data present in the scene file
	tower_scene_->CreateMaterials(*platform_);
	bomb_scene_->CreateMaterials(*platform_);

	// if there is mesh data in the scene, create a mesh to draw from the first mesh
	tower_mesh_ = GetFirstMesh(tower_scene_, platform_);
	bomb_mesh_ = GetFirstMesh(bomb_scene_, platform_);

	// Set textures
	blue_bomb_tex = CreateTextureFromPNG("blue_bombman_tex.png", *platform_);
	red_bomb_tex = CreateTextureFromPNG("red_bombman_tex.png", *platform_);

	tower_tex = CreateTextureFromPNG("tower_texture_.png", *platform_);

	lightning_tex = CreateTextureFromPNG("lightning1.png", *platform_);

	character_scale.set_value(0.01f, 0.01f, 0.01f);

	mesh_matrix.SetIdentity();
	mesh_matrix.Scale(gef::Vector4(0.001f, 0.001f, 0.001f));

	// Set cubes
	for (int i = 0; i < 3; i++)
	{
		beam_cube[i].set_mesh(primitive_builder_->GetDefaultCubeMesh());
		beam_cube[i].set_transform(mesh_matrix);

		tower[i].set_mesh(tower_mesh_);
		tower->set_transform(mesh_matrix);
	}

	// Loop for 10 characters
	for (int i = 0; i < 10; i++)
	{
		character.push_back(SpawnCharacter());
	}

	// Set the time limit
	time_limit = 60.0f;
	// Set up timer
	timer = time_limit;

	//load in audio assets
	if (audio_manager_)
	{
		zap_id = audio_manager_->LoadSample("zap.wav", *platform_);
	}
}

bool PlayState::Update(float frame_time, gef::InputManager* input_manager_, ARApp* main_app, int& score, gef::AudioManager* audio_manager_)
{
	fps_ = 1.0f / frame_time;

	// Decrease timer
	timer -= frame_time;

	// Check if time is up
	if (timer <= 0.0f)
	{
		// randomise characters for next time
		RandomiseCharacters();

		// Reset the timer
		timer = time_limit;

		// Change to end state
		main_app->Set_State(2);

		return true;
	}

	AppData* dat = sampleUpdateBegin();

	// use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	// check to see if marker 4 can be found
	if (sampleIsMarkerFound(3))
	{
		// marker is being tracked, get it’s transform
		gef::Matrix44 marker_transform;
		sampleGetTransform(3, &marker_transform);

		// Marker was found
		centre.SetFound(true);

		// Save the matrix 
		centre.SetTransform(marker_transform);

		// Loop for all characters
		for (int i = 0; i < character.size(); i++)
		{
			// set the transform of the 3D mesh instance to draw on top of the marker
			character[i].set_transform(marker_transform);
		}
	}
	else
	{
		// Marker was not found
		centre.SetFound(false);
	}

	// Loop for the three markers
	for (int i = 0; i < 3; i++)
	{
		// check to see if marker i can be found
		if (sampleIsMarkerFound(i))
		{
			// marker is being tracked, get it’s transform
			gef::Matrix44 marker_transform;
			sampleGetTransform(i, &marker_transform);

			// Marker was found
			marker[i].SetFound(true);

			// Save the matrix 
			marker[i].SetTransform(marker_transform);
			
			// Calculate the local transform using the origin transform
			marker[i].SetLocalTransform(centre.GetTransform());

			// Place tower on marker
			tower[i].set_transform(mesh_matrix * marker_transform);

			// set the transform of the 3D mesh instance to draw on top of the marker
			beam_cube[i].set_transform(marker_transform);
		}
		else
		{
			// Marker was not found
			marker[i].SetFound(false);
		}
	}

	// Nested for loop
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// Check if both were found
			if (marker[i].GetFound() && marker[j].GetFound())
			{
				// Set direction vector
				direction[i][j] = marker[j].GetLocalTransform().GetTranslation() - marker[i].GetLocalTransform().GetTranslation();
			}
		}
	}

	// Nested for loop
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// Check if marker is missing
			if (!marker[j].GetFound() && marker[i].GetFound())
			{
				// Translate to predicted position
				gef::Matrix44 temp;
				temp.SetIdentity();
				temp.SetTranslation(direction[i][j]);
				temp = marker[i].GetLocalTransform() * temp;

				marker[j].SetTransform(temp);

				// Place tower on marker
				tower[j].set_transform(mesh_matrix * marker[j].GetTransform());
			}
		}
	}


	for (int i = 0; i < 3; i++)
	{
		int j = i + 1;
		if (j >= 3)
		{
			j = 0;
		}

		// Set values for AB
		v_dist = marker[j].GetLocalTransform().GetTranslation().y() - marker[i].GetLocalTransform().GetTranslation().y();
		h_dist = marker[j].GetLocalTransform().GetTranslation().x() - marker[i].GetLocalTransform().GetTranslation().x();
		angle = atanf(h_dist / v_dist);

		// Pythagoras
		dist = sqrt(pow(h_dist, 2) + pow(v_dist, 2));

		// Find midpoints
		gef::Vector4 midpoint = ((marker[i].GetLocalTransform().GetTranslation() + marker[j].GetLocalTransform().GetTranslation()) / 2) + gef::Vector4(0, 0, 0.1f);

		// Create a temporary transform
		gef::Matrix44 offset_matrix;
		gef::Matrix44 offset_matrix_scale;
		gef::Matrix44 offset_matrix_rot;
		gef::Matrix44 offset_matrix_trans;

		// Set beam scale
		gef::Vector4 beam_scale(0.02f, dist, 0.02f);

		// Set the scale, rotation and translation
		offset_matrix_scale.SetIdentity();
		offset_matrix_scale.Scale(beam_scale);
		offset_matrix_rot.SetIdentity();
		offset_matrix_rot.RotationZ(-angle);
		offset_matrix_trans.SetIdentity();
		offset_matrix_trans.SetTranslation(midpoint);

		// Set offset
		offset_matrix.SetIdentity();
		offset_matrix = offset_matrix_scale * offset_matrix_rot * offset_matrix_trans;

		// Set the transform
		beam_cube[i].set_transform(offset_matrix * centre.GetTransform());
	}

	// Create a matrix for the character
	gef::Matrix44 character_matrix;
	gef::Matrix44 character_matrix_scale;
	gef::Matrix44 character_matrix_rot;
	gef::Matrix44 character_matrix_trans;

	// Set to identity
	character_matrix.SetIdentity();
	character_matrix_scale.SetIdentity();
	character_matrix_rot.SetIdentity();
	character_matrix_trans.SetIdentity();
	
	// Check if initial has been set
	if (centre.GetFound())
	{
		// Loop for all characters
		for (int i = 0; i < 10; i++)
		{
			// Set the position
			character_matrix_scale.Scale(character_scale);
			character_matrix_rot.RotationX(PI / 2);
			character_matrix_trans.SetTranslation(character[i].GetDirection());

			// Combine
			character_matrix = character_matrix_scale * character_matrix_rot * character_matrix_trans;

			// Apply to character
			character[i].set_transform(character_matrix * centre.GetTransform());
		}
	}

	// Check for cross pressed
	input_manager_->Update();
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (controller)
	{
		// Check for X to be pressed
		if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
		{
			// Play sound
			//gef::VolumeInfo zap_volume_info;
			//zap_volume_info.volume = 0.1f;
			//audio_manager_->SetMusicVolumeInfo(zap_volume_info);
			audio_manager_->PlaySample(zap_id, false);

			// create triangle 
			gef::Vector4 corner[3];
			corner[0] = marker[0].GetTransform().GetTranslation();
			corner[1] = marker[1].GetTransform().GetTranslation();
			corner[2] = marker[2].GetTransform().GetTranslation();

			// Loop for all characters
			for (int i = 0; i < character.size(); i++)
			{
				// Check collision
				gef::Vector4 test;
				test = character[i].transform().GetTranslation();
				if (collision_manager.RayTriangleCollision(test , corner))
				{
					// Add character points to score
					score += character[i].GetPoints();

					// Kill character
					character.erase(character.begin() + i);

					// Create a new character (front so they don't get killed)
					character.insert(character.begin(), SpawnCharacter());
				}
			}
		}
	}

	sampleUpdateEnd(dat);

	return false;
}

void PlayState::Render(gef::Matrix44 proj_matrix, gef::Matrix44 view_matrix, gef::SpriteRenderer* sprite_renderer_, gef::TextureVita& tex_vita, gef::Renderer3D* renderer_3d_, float cam_aspect_ratio, float display_aspect_ratio, float vert_scale_factor, gef::Sprite cam_sprite, gef::Font* font_, gef::Platform& platform_, int score)
{
	AppData* dat = sampleRenderBegin();

	//
	// Render the camera feed
	//

	// REMEMBER AND SET THE PROJECTION MATRIX HERE
	// Set up proj matrix
	// TODO maybe need to change aspect ratio
	proj_matrix.PerspectiveFovGL(SCE_SMART_IMAGE_FOV, cam_aspect_ratio, 0.01f, 100.0f);

	gef::Matrix44 temp_scale;
	temp_scale.Scale(gef::Vector4(1, vert_scale_factor, 1));
	proj_matrix = temp_scale * proj_matrix;

	// Set view matrix to identity
	view_matrix.SetIdentity();

	sprite_renderer_->Begin(true);

	// DRAW CAMERA FEED SPRITE HERE
	//sprite_renderer_->DrawSprite(cam_sprite);

	// check there is data present for the camera image before trying to draw it
	if (dat->currentImage)
	{
		tex_vita.set_texture(dat->currentImage->tex_yuv);
		sprite_renderer_->DrawSprite(cam_sprite);
	}

	sprite_renderer_->End();

	//
	// Render 3D scene
	//

	// SET VIEW AND PROJECTION MATRIX HERE

	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d_->Begin(false);

	// Set projection matrix of the renderer
	renderer_3d_->set_projection_matrix(proj_matrix);
	renderer_3d_->set_view_matrix(view_matrix);

	// DRAW 3D MESHES HERE
	for (int i = 0; i < 3; i++)
	{
		lightning_mat.set_texture(lightning_tex);
		renderer_3d_->set_override_material(&lightning_mat);
		renderer_3d_->DrawMesh(beam_cube[i]);

		tower_mat.set_texture(tower_tex);
		renderer_3d_->set_override_material(&tower_mat);
		renderer_3d_->DrawMesh(tower[i]);
	}

	//renderer_3d_->DrawMesh(beam_cube[2]);

	// Check if centre marker is visible
	if (centre.GetFound())
	{
		// Loop for all characters
		for (int i = 0; i < 10; i++)
		{
			// Draw the character
			bomb_mat.set_texture(character[i].GetTex());
			renderer_3d_->set_override_material(&bomb_mat);
			renderer_3d_->DrawMesh(character[i]);
		}
	}

	renderer_3d_->End();

	RenderOverlay(sprite_renderer_, platform_, font_, score);

	sampleRenderEnd();
}

void PlayState::RenderOverlay(gef::SpriteRenderer* sprite_renderer_, gef::Platform& platform_, gef::Font* font_, int score)
{
	//
	// render 2d hud on top
	//
	gef::Matrix44 proj_matrix2d;

	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);
	sprite_renderer_->Begin(false);
	DrawHUD(font_, sprite_renderer_, score);
	sprite_renderer_->End();
}

void PlayState::DrawHUD(gef::Font* font_, gef::SpriteRenderer* sprite_renderer_, int score)
{
	if (font_)
	{
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
		font_->RenderText(sprite_renderer_, gef::Vector4(0.0f, 0.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Time: %.1f", timer);
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 0.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, "Score: %i", score);
	}
}

Character PlayState::SpawnCharacter()
{
	// Create a new character
	Character new_character;
	new_character.set_mesh(bomb_mesh_);

	// Randomise enemies and friendlies
	if (rand() % 100 > 20)
	{
		// Set up enemy
		new_character.SetPoints(10.0f);
		new_character.SetTex(red_bomb_tex);
	}
	else
	{
		// Set up friend
		new_character.SetPoints(-100.0f);
		new_character.SetTex(blue_bomb_tex);
	}

	// Set up floats
	float rand_x = rand() % 100 - 50;
	float rand_y = rand() % 100 - 50;

	// Divide 
	rand_x /= 200.0f;
	rand_y /= 200.0f;

	// Set character positions
	new_character.SetDirection(gef::Vector4(rand_x, rand_y, 0.0f));

	return new_character;
}

void PlayState::RandomiseCharacters()
{
	for (int i = 0; i < character.size(); i++)
	{
		// Randomise enemies and friendlies
		if (rand() % 100 > 20)
		{
			// Set up enemy
			character[i].SetPoints(10.0f);
			character[i].SetTex(red_bomb_tex);
		}
		else
		{
			// Set up friend
			character[i].SetPoints(-100.0f);
			character[i].SetTex(blue_bomb_tex);
		}

		// Set up floats
		float rand_x = rand() % 100 - 50;
		float rand_y = rand() % 100 - 50;

		// Divide 
		rand_x /= 200.0f;
		rand_y /= 200.0f;

		// Set character positions
		character[i].SetDirection(gef::Vector4(rand_x, rand_y, 0.0f));
	}
}

gef::Mesh* PlayState::GetFirstMesh(gef::Scene* scene, gef::Platform* platform_)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
			mesh = scene->CreateMesh(*platform_, scene->mesh_data.front());
	}

	return mesh;
}

gef::Texture* PlayState::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
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
