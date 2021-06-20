#include "Character.h"

Character::Character()
{
	GameObject();
}

Character::~Character()
{
}

gef::Vector4 Character::GetDirection()
{
	return direction;
}

void Character::SetDirection(gef::Vector4 new_direction)
{
	direction = new_direction;
}

float Character::GetPoints()
{
	return point_value;
}

void Character::SetPoints(float new_value)
{
	point_value = new_value;
}

gef::Texture * Character::GetTex()
{
	return character_texture;
}

void Character::SetTex(gef::Texture * new_texture)
{
	character_texture = new_texture;
}
