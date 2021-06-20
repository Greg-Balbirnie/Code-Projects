#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>

class Collision
{
public:
	Collision();
	~Collision();
	bool collide(sf::CircleShape, sf::CircleShape);
};

