#pragma once
#include <system/platform.h>
#include "GameObject.h"

class Character : public GameObject
{
public:
	Character();
	~Character();

	gef::Vector4 GetDirection();
	void SetDirection(gef::Vector4 new_direction);

	float GetPoints();
	void SetPoints(float new_value);

	gef::Texture* GetTex();
	void SetTex(gef::Texture* new_texture);

private:
	gef::Vector4 direction;	// The direction vector from the initial marker to this character
	float point_value;	// The amount of points killing the character will award the player
	gef::Texture* character_texture;	// The texture for the character
};

